#!/usr/bin/env python3

import argparse
import json
import pathlib
import subprocess
import sys

def run_benchmark(executable: str, samples: int, warmups: int) -> list[dict]:
    proc = subprocess.run(
        [executable, "--json", f"--samples={samples}", f"--warmups={warmups}"],
        check=False,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"{executable} failed with exit code {proc.returncode}\n"
            f"stdout:\n{proc.stdout}\n"
            f"stderr:\n{proc.stderr}"
        )

    try:
        payload = json.loads(proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(
            f"{executable} did not emit valid JSON\nstdout:\n{proc.stdout}"
        ) from exc

    if not isinstance(payload, list):
        raise RuntimeError(f"{executable} JSON payload must be a list")
    return payload


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Icey benchmark executables")
    parser.add_argument("--output", required=True, help="Path to write the aggregated JSON report")
    parser.add_argument("--samples", type=int, default=3, help="Number of timed samples per benchmark")
    parser.add_argument("--warmups", type=int, default=1, help="Number of warmup samples per benchmark")
    parser.add_argument("executables", nargs="+", help="Benchmark executables to run")
    args = parser.parse_args()

    report: dict[str, list[dict]] = {}
    for executable in args.executables:
        path = pathlib.Path(executable)
        report[path.stem] = run_benchmark(executable, args.samples, args.warmups)

    output_path = pathlib.Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")

    print(f"wrote benchmark report: {output_path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
