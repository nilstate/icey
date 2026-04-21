#!/usr/bin/env python3

import argparse
import json
import math
import pathlib
import sys

REQUIRED_SUITES = (
    "signalbench",
    "packetstreambench",
    "httpbench",
    "httpparsebench",
    "wsbench",
    "speechbench",
    "stunbench",
    "symplebench",
    "visionbench",
)

REQUIRED_RESULTS = {
    "speechbench": (
        "speech energy vad fltp",
    ),
    "visionbench": (
        "vision sampler forward every frame",
        "vision sampler every fourth frame",
        "vision sampler 50ms gate",
        "vision motion detector",
    ),
}

FFMPEG_REQUIRED_SUITES = ("speechbench", "visionbench")


def is_number(value: object) -> bool:
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def load_report(path: pathlib.Path) -> dict[str, list[dict]]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError("benchmark report must be a JSON object keyed by benchmark suite")
    return payload


def flatten_results(report: dict[str, list[dict]]) -> dict[tuple[str, str], dict]:
    flattened: dict[tuple[str, str], dict] = {}
    for suite, results in report.items():
        if not isinstance(results, list):
            continue
        for result in results:
            if not isinstance(result, dict):
                continue
            name = result.get("name")
            if isinstance(name, str):
                flattened[(suite, name)] = result
    return flattened


def validate_result_shape(suite: str, result: dict, errors: list[str]) -> None:
    if not isinstance(result, dict):
        errors.append(f"{suite}: benchmark entry must be an object")
        return

    name = result.get("name")
    if not isinstance(name, str) or not name:
        errors.append(f"{suite}: benchmark entry is missing a non-empty name")
        return

    metrics = result.get("metrics")
    if not isinstance(metrics, dict):
        errors.append(f"{suite}/{name}: metrics must be a JSON object")
        return

    for field in ("ns_per_op", "best_ns_per_op", "iterations", "samples"):
        value = result.get(field)
        if not is_number(value):
            errors.append(f"{suite}/{name}: {field} must be numeric")
            return

    ns_per_op = float(result["ns_per_op"])
    best_ns_per_op = float(result["best_ns_per_op"])
    iterations = int(result["iterations"])
    samples = int(result["samples"])

    if not math.isfinite(ns_per_op) or not math.isfinite(best_ns_per_op):
        errors.append(f"{suite}/{name}: benchmark timings must be finite")

    if samples < 0 or iterations < 0:
        errors.append(f"{suite}/{name}: samples and iterations must not be negative")

    if samples > 0 or iterations > 0:
        if samples <= 0 or iterations <= 0:
            errors.append(f"{suite}/{name}: non-empty benchmarks must report positive samples and iterations")
        if ns_per_op <= 0 or best_ns_per_op <= 0:
            errors.append(f"{suite}/{name}: non-empty benchmarks must report positive timings")
        if best_ns_per_op > ns_per_op:
            errors.append(f"{suite}/{name}: best_ns_per_op must not exceed median ns_per_op")


def compare_with_baseline(
    report: dict[str, list[dict]],
    baseline: dict[str, list[dict]],
    max_regression_pct: float,
    errors: list[str],
) -> int:
    current_results = flatten_results(report)
    baseline_results = flatten_results(baseline)
    compared = 0

    for key, current in current_results.items():
        previous = baseline_results.get(key)
        if previous is None:
            continue

        current_ns = float(current.get("ns_per_op", 0.0))
        previous_ns = float(previous.get("ns_per_op", 0.0))
        if current_ns <= 0 or previous_ns <= 0:
            continue

        compared += 1
        allowed = previous_ns * (1.0 + max_regression_pct / 100.0)
        if current_ns > allowed:
            suite, name = key
            errors.append(
                f"{suite}/{name}: ns_per_op regressed from {previous_ns:.3f} to {current_ns:.3f}"
                f" (> {max_regression_pct:.1f}% allowed)"
            )

    return compared


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate an aggregated icey benchmark report")
    parser.add_argument("--report", required=True, help="Path to benchmarks.json")
    parser.add_argument(
        "--baseline",
        help="Optional baseline benchmarks.json to compare against",
    )
    parser.add_argument(
        "--max-regression-pct",
        type=float,
        default=0.0,
        help="Allowed ns_per_op regression percentage when --baseline is provided",
    )
    args = parser.parse_args()

    report_path = pathlib.Path(args.report)
    report = load_report(report_path)
    errors: list[str] = []

    for suite in REQUIRED_SUITES:
        results = report.get(suite)
        if not isinstance(results, list) or not results:
            errors.append(f"{suite}: missing or empty benchmark suite")
            continue

        for result in results:
            validate_result_shape(suite, result, errors)

        names = {
            result.get("name")
            for result in results
            if isinstance(result, dict) and isinstance(result.get("name"), str)
        }
        for required_name in REQUIRED_RESULTS.get(suite, ()):
            if required_name not in names:
                errors.append(f"{suite}: missing required benchmark '{required_name}'")

        if suite in FFMPEG_REQUIRED_SUITES:
            for result in results:
                if not isinstance(result, dict):
                    continue
                metrics = result.get("metrics")
                if isinstance(metrics, dict) and metrics.get("path") == "no_ffmpeg":
                    errors.append(f"{suite}: reported no_ffmpeg fallback instead of FFmpeg-backed benchmarks")

    compared = 0
    if args.baseline:
        baseline_path = pathlib.Path(args.baseline)
        baseline = load_report(baseline_path)
        compared = compare_with_baseline(report, baseline, args.max_regression_pct, errors)

    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    flattened = flatten_results(report)
    speech = flattened[("speechbench", "speech energy vad fltp")]
    motion = flattened[("visionbench", "vision motion detector")]
    print(
        "validated benchmark report:",
        report_path,
        f"(suites={len(REQUIRED_SUITES)}, compared={compared},"
        f" speech_ns_per_op={float(speech['ns_per_op']):.3f},"
        f" motion_ns_per_op={float(motion['ns_per_op']):.3f})",
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
