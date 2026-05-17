#!/usr/bin/env python3
"""Compare DECtalk WAV baselines with current generated WAV files."""

from __future__ import annotations

import argparse
import hashlib
import math
import struct
import sys
import wave
from pathlib import Path


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def read_wav(path: Path) -> tuple[wave._wave_params, list[int], bytes]:
    with wave.open(str(path), "rb") as handle:
        params = handle.getparams()
        data = handle.readframes(params.nframes)

    if params.nchannels != 1:
        raise ValueError(f"{path}: expected mono WAV, got {params.nchannels} channels")
    if params.sampwidth != 2:
        raise ValueError(f"{path}: expected 16-bit samples, got {params.sampwidth * 8}-bit")

    count = len(data) // 2
    samples = list(struct.unpack(f"<{count}h", data))
    return params, samples, data


def rms(samples: list[int]) -> float:
    if not samples:
        return 0.0
    return math.sqrt(sum(sample * sample for sample in samples) / len(samples))


def compare_pair(expected: Path, actual: Path) -> list[str]:
    problems: list[str] = []
    expected_params, expected_samples, expected_data = read_wav(expected)
    actual_params, actual_samples, actual_data = read_wav(actual)

    if expected_params != actual_params:
        problems.append(f"params differ: expected {expected_params}, actual {actual_params}")

    expected_hash = sha256(expected)
    actual_hash = sha256(actual)
    if expected_hash == actual_hash:
        return problems

    problems.append(f"sha256 differs: expected {expected_hash}, actual {actual_hash}")

    if len(expected_samples) != len(actual_samples):
        problems.append(
            f"sample count differs: expected {len(expected_samples)}, actual {len(actual_samples)}"
        )

    paired_count = min(len(expected_samples), len(actual_samples))
    if paired_count:
        diffs = [
            abs(expected_samples[index] - actual_samples[index])
            for index in range(paired_count)
        ]
        problems.append(f"max abs sample delta: {max(diffs)}")
        problems.append(f"expected peak/rms: {max(map(abs, expected_samples))}/{rms(expected_samples):.2f}")
        problems.append(f"actual peak/rms: {max(map(abs, actual_samples))}/{rms(actual_samples):.2f}")
    elif expected_data != actual_data:
        problems.append("audio payload differs and no samples were available to compare")

    return problems


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--expected",
        type=Path,
        default=Path("tests/golden/audio/us"),
        help="directory containing baseline WAV files",
    )
    parser.add_argument(
        "--actual",
        type=Path,
        required=True,
        help="directory containing newly generated WAV files",
    )
    args = parser.parse_args()

    expected_files = sorted(args.expected.glob("*.wav"))
    if not expected_files:
        print(f"error: no WAV files found in {args.expected}", file=sys.stderr)
        return 2

    failed = False
    for expected in expected_files:
        actual = args.actual / expected.name
        if not actual.exists():
            print(f"{expected.name}: missing actual file {actual}")
            failed = True
            continue
        problems = compare_pair(expected, actual)
        if problems:
            failed = True
            print(f"{expected.name}: DIFFER")
            for problem in problems:
                print(f"  {problem}")
        else:
            print(f"{expected.name}: OK")

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
