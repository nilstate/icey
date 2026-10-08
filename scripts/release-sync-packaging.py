#!/usr/bin/env python3
"""Sync dated Debian and openSUSE release records from the source changelog."""

from datetime import date
from pathlib import Path
import re
import sys


root = Path(sys.argv[1]).resolve()
version = sys.argv[2]
if not re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+", version):
    raise SystemExit("expected MAJOR.MINOR.PATCH version")

changelog = (root / "CHANGELOG.md").read_text()
match = re.search(rf"^## \[{re.escape(version)}\] - ([0-9]{{4}}-[0-9]{{2}}-[0-9]{{2}})$", changelog, re.M)
if not match:
    raise SystemExit(f"CHANGELOG.md [{version}] needs an ISO release date")
release_day = date.fromisoformat(match.group(1))

spec = root / "packaging/opensuse/icey/icey.spec"
source, count = re.subn(r"^Version:\s+\d+\.\d+\.\d+$", f"Version:        {version}", spec.read_text(), flags=re.M)
if count != 1:
    raise SystemExit("expected one openSUSE spec version")
spec.write_text(source)

service = root / "packaging/opensuse/icey/_service"
source, count = re.subn(r'(<param name="revision">)\d+\.\d+\.\d+(</param>)', rf"\g<1>{version}\g<2>", service.read_text())
if count != 1:
    raise SystemExit("expected one openSUSE service revision")
service.write_text(source)

changes = root / "packaging/opensuse/icey/icey.changes"
source = changes.read_text()
latest = re.search(r"^- Update icey to ([0-9]+\.[0-9]+\.[0-9]+)$", source, re.M)
if not latest or latest.group(1) != version:
    entry = ("-------------------------------------------------------------------\n"
             f"{release_day:%a %b %d} 00:00:00 UTC {release_day:%Y} - 0state OSS <oss@0state.com>\n\n"
             f"- Update icey to {version}\n\n")
    changes.write_text(entry + source)

readme = root / "packaging/opensuse/README.md"
source, count = re.subn(r'^(- `icey/`: OBS service, spec, and changelog for `icey) [0-9]+\.[0-9]+\.[0-9]+(`)$', r"\1\2", readme.read_text(), flags=re.M)
if count > 1:
    raise SystemExit("expected at most one openSUSE README version")
readme.write_text(source)

debian = root / "packaging/debian/debian/changelog"
source = debian.read_text()
if not re.match(rf"icey \({re.escape(version)}-1\) ", source):
    source, count = re.subn(r"^icey \(\d+\.\d+\.\d+-\d+\) ", f"icey ({version}-1) ", source, count=1)
    if count != 1:
        raise SystemExit("expected one Debian changelog header")
    stamp = release_day.strftime("%a, %d %b %Y 00:00:00 +0000")
    source, count = re.subn(r"^ -- .*?$", f" -- 0state OSS <oss@0state.com>  {stamp}", source, count=1, flags=re.M)
    if count != 1:
        raise SystemExit("expected one Debian changelog signature")
    debian.write_text(source)
