#!/usr/bin/env python3
"""Create a reproducible Debian orig tarball from an exact staged source tree."""

import gzip
from pathlib import Path
import sys
import tarfile


source = Path(sys.argv[1]).resolve()
output = Path(sys.argv[2]).resolve()
epoch = int(sys.argv[3])

with output.open("wb") as stream:
    with gzip.GzipFile(fileobj=stream, mode="wb", filename="", mtime=0) as compressed:
        with tarfile.open(fileobj=compressed, mode="w|", format=tarfile.PAX_FORMAT) as archive:
            for path in [source, *sorted(source.rglob("*"))]:
                relative = path.relative_to(source)
                if relative.parts and relative.parts[0] == "debian":
                    continue
                name = (Path(source.name) / relative).as_posix()
                info = archive.gettarinfo(str(path), arcname=name)
                info.mtime = epoch
                info.uid = info.gid = 0
                info.uname = info.gname = ""
                if info.isfile():
                    with path.open("rb") as data:
                        archive.addfile(info, data)
                else:
                    archive.addfile(info)
