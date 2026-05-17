#!/usr/bin/env python3
"""Summarize DECtalk compiler warnings by category and file.

Purpose: turn GCC-style build logs into stable warning inventory reports.
Scope: baseline and planning support only; this script does not modify source or
build outputs.
Behavior preservation: warning cleanup can be planned by category without
touching synthesis, dictionary, audio, or public API code prematurely.
Limitations: category extraction is regex-based and tuned for GCC-style warning
lines emitted by the current Linux build.
"""

from __future__ import annotations

import argparse
import collections
import csv
import os
import re
from pathlib import Path


MAKE_ENTER_RE = re.compile(r"^make(?:\[(?P<level>\d+)\])?: Entering directory '(?P<path>.*)'$")
MAKE_LEAVE_RE = re.compile(r"^make(?:\[(?P<level>\d+)\])?: Leaving directory '(?P<path>.*)'$")
WARNING_RE = re.compile(
    r"^(?P<path>[^:\n]+):(?P<line>\d+):(?:(?P<column>\d+):)? warning: "
    r"(?P<message>.*?)(?: \[(?P<flag>-W[^\]]+)\])?$"
)


def normalize_message(message: str) -> str:
    message = re.sub(r"[`'‘’][^`'‘’]+[`'‘’]", "<name>", message)
    message = re.sub(r"\b\d+\b", "<n>", message)
    message = re.sub(r"\s+", " ", message)
    return message.strip()


def normalize_path(path: str, current_dir: Path | None, repo_root: Path) -> str:
    raw_path = Path(path)
    if not raw_path.is_absolute() and current_dir is not None:
        raw_path = current_dir / raw_path
    normalized = raw_path.resolve(strict=False)
    try:
        return normalized.relative_to(repo_root).as_posix()
    except ValueError:
        return os.path.normpath(path).replace("\\", "/")


def risk_for(path: str, flag: str, message: str) -> str:
    high_risk_parts = (
        "src/dapi/src/hlsyn/",
        "src/dapi/src/lts/",
        "src/dapi/src/ph/",
        "src/dapi/src/vtm/",
        "src/dapi/src/api/ttsapi.c",
        "src/dapi/src/api/init.c",
        "src/dapi/src/nt/opthread.c",
        "src/dapi/src/nt/linux_audio.c",
    )
    medium_flags = {
        "-Wpointer-to-int-cast",
        "-Wint-to-pointer-cast",
        "-Wcast-qual",
        "-Wdiscarded-qualifiers",
        "-Wincompatible-pointer-types",
        "-Wconversion",
        "-Wsign-conversion",
    }
    low_flags = {
        "-Wunused-parameter",
        "-Wunused-variable",
        "-Wunused-function",
        "-Wmissing-prototypes",
        "-Wmissing-declarations",
        "-Wformat",
        "-Wformat=",
        "-Wold-style-definition",
    }

    normalized_path = path.replace("\\", "/")
    if any(part in normalized_path for part in high_risk_parts):
        return "high"
    if flag in medium_flags or "volatile" in message or "callback" in message:
        return "medium"
    if flag in low_flags or flag.startswith("-Wformat"):
        return "low"
    return "unknown"


def read_warnings(log_path: Path, repo_root: Path) -> list[dict[str, str]]:
    warnings: list[dict[str, str]] = []
    make_dirs: dict[int, Path] = {}
    for line in log_path.read_text(errors="replace").splitlines():
        enter_match = MAKE_ENTER_RE.match(line)
        if enter_match:
            level = int(enter_match.group("level") or 0)
            make_dirs[level] = Path(enter_match.group("path"))
            continue

        leave_match = MAKE_LEAVE_RE.match(line)
        if leave_match:
            level = int(leave_match.group("level") or 0)
            make_dirs.pop(level, None)
            continue

        match = WARNING_RE.match(line)
        if not match:
            continue
        data = match.groupdict()
        current_dir = make_dirs[max(make_dirs)] if make_dirs else None
        data["path"] = normalize_path(data["path"], current_dir, repo_root)
        flag = data.get("flag") or "unflagged"
        message = data["message"]
        data["flag"] = flag
        data["category"] = normalize_message(message)
        data["risk"] = risk_for(data["path"], flag, message)
        warnings.append(data)
    return warnings


def write_counter(path: Path, header: list[str], rows: list[tuple[object, ...]]) -> None:
    with path.open("w", newline="") as handle:
        writer = csv.writer(handle, delimiter="\t")
        writer.writerow(header)
        writer.writerows(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--log", type=Path, required=True, help="build log to parse")
    parser.add_argument("--out-dir", type=Path, required=True, help="directory for TSV reports")
    parser.add_argument(
        "--repo-root",
        type=Path,
        default=Path.cwd(),
        help="repository root used to normalize warning paths",
    )
    args = parser.parse_args()

    warnings = read_warnings(args.log, args.repo_root.resolve(strict=False))
    args.out_dir.mkdir(parents=True, exist_ok=True)

    by_flag = collections.Counter(warning["flag"] for warning in warnings)
    by_category = collections.Counter(warning["category"] for warning in warnings)
    by_file = collections.Counter(warning["path"] for warning in warnings)
    by_risk = collections.Counter(warning["risk"] for warning in warnings)

    write_counter(
        args.out_dir / "by-flag.tsv",
        ["count", "flag"],
        [(count, flag) for flag, count in by_flag.most_common()],
    )
    write_counter(
        args.out_dir / "by-category.tsv",
        ["count", "category"],
        [(count, category) for category, count in by_category.most_common()],
    )
    write_counter(
        args.out_dir / "by-file.tsv",
        ["count", "file"],
        [(count, file) for file, count in by_file.most_common()],
    )
    write_counter(
        args.out_dir / "by-risk.tsv",
        ["count", "risk"],
        [(count, risk) for risk, count in by_risk.most_common()],
    )

    with (args.out_dir / "warnings.tsv").open("w", newline="") as handle:
        writer = csv.writer(handle, delimiter="\t")
        writer.writerow(["file", "line", "column", "flag", "risk", "category", "message"])
        for warning in warnings:
            writer.writerow(
                [
                    warning["path"],
                    warning["line"],
                    warning.get("column") or "",
                    warning["flag"],
                    warning["risk"],
                    warning["category"],
                    warning["message"],
                ]
            )

    print(f"warnings={len(warnings)}")
    print(f"out_dir={args.out_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
