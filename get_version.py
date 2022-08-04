#!/usr/bin/env python3

import os
import sys
from shutil import which
from subprocess import check_output

def main():
	srcdir = os.path.dirname(os.path.abspath(sys.argv[0]))
	git_dir = os.path.join(srcdir, ".git")
	git = which('git')

	if git and os.path.exists(git_dir):
		version = check_output([git, "-C", srcdir, "describe", "--tags"],
	                			encoding="UTF-8")
		version = version.strip()
	else:
		version = '1.4.4'
	print(version)

if __name__ == "__main__":
	main()
