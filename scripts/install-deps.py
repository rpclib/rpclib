#! /usr/bin/env python3

import subprocess as sp
import os

from pathlib import Path


cwd = Path.cwd()

if ".gitignore" in list(cwd.iterdir()):
  raise RuntimeError("Run this script from a build directory.")


sp.run(["conan", "install", ".."]).check_returncode()

# This is a workaround for https://github.com/conan-io/conan-center-index/issues/237
def zlib_workaround():
  os.rename("FindZLIB.cmake", "Findzlib.cmake")
  contents = open("Findzlib.cmake").readlines()
  with open("Findzlib.cmake", "w") as fp:
    for line in contents:
      fp.write(line.replace("ZLIB", "zlib"))

zlib_workaround()
