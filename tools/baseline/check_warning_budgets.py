#!/usr/bin/env python3
"""Check DECtalk warning budgets for cleaned warning categories.

Purpose: prevent regressions in warning categories that have already been
cleaned without making all legacy warning debt fatal.
Scope: baseline and CI verification for GCC-style warning TSV output generated
by summarize_warnings.py.
Behavior preservation: this script only reads warning reports and budget files;
it does not change compiler flags, source code, or runtime behavior.
Limitations: budgets are intentionally narrow and should be added only for
reviewed files/categories.
"""

from __future__ import annotations

import argparse
import csv
from collections import Counter
from pathlib import Path


def read_warning_counts(warnings_file: Path) -> Counter[tuple[str, str]]:
    counts: Counter[tuple[str, str]] = Counter()
    with warnings_file.open(newline="") as handle:
        reader = csv.DictReader(handle, delimiter="\t")
        for row in reader:
            counts[(row["file"], row["flag"])] += 1
    return counts


def read_budgets(budget_file: Path) -> list[dict[str, str]]:
    with budget_file.open(newline="") as handle:
        reader = csv.DictReader(handle, delimiter="\t")
        return list(reader)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--warnings", type=Path, required=True, help="warnings.tsv")
    parser.add_argument("--budget", type=Path, required=True, help="budget TSV")
    parser.add_argument("--out", type=Path, required=True, help="result TSV")
    args = parser.parse_args()

    counts = read_warning_counts(args.warnings)
    budgets = read_budgets(args.budget)
    args.out.parent.mkdir(parents=True, exist_ok=True)

    failed = False
    with args.out.open("w", newline="") as handle:
        writer = csv.writer(handle, delimiter="\t")
        writer.writerow(["file", "flag", "max_count", "actual_count", "status", "note"])
        for budget in budgets:
            file_name = budget["file"]
            flag = budget["flag"]
            max_count = int(budget["max_count"])
            actual_count = counts[(file_name, flag)]
            status = "ok" if actual_count <= max_count else "fail"
            if status == "fail":
                failed = True
            writer.writerow(
                [
                    file_name,
                    flag,
                    max_count,
                    actual_count,
                    status,
                    budget.get("note", ""),
                ]
            )

    print(f"warning_budget={args.out}")
    if failed:
        print("warning_budget_status=fail")
        return 1
    print("warning_budget_status=ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
