#!/usr/bin/env python3

# This file is part of the GlyphVisualizer project, a Glyph composition
# player that plays Glyph compositions from Nothing phones.
# Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# Inspired by: https://github.com/smichard/conventional_changelog

import sys

# Check the python version
if sys.version_info < (3, 10):
    print("This script requires Python 3.10 or higher! Please upgrade your python version and try again.")
    sys.exit(1)

import argparse
import os
import re
import subprocess
import shutil
import time


# +------------------------------------+
# |                                    |
# |              Globals               |
# |                                    |
# +------------------------------------+

# Get the script directory
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

# Relative path to the repository
REPO_PATH = os.path.join(SCRIPT_DIR, '../')

BUGFIX_CATEGORY_REGEX = re.compile(r'^fix(\(.*\))?!?: (.*)$')
FEATURE_CATEGORY_REGEX = re.compile(r'^feat(\(.*\))?!?: (.*)$')

# +------------------------------------+
# |                                    |
# |           Bioler Plate             |
# |                                    |
# +------------------------------------+

def build_arguments_parser() -> argparse.ArgumentParser:
    # Parse the arguments with argparse (https://docs.python.org/3/library/argparse.html)
    parser = argparse.ArgumentParser(add_help=False, description="Create release notes through conventional commits.", epilog="Created by: Sebastian Aigner (aka. SebiAi)")

    # Add the arguments
    parser.add_argument('-h', '--help', action='help', help='Show this help message and exit.') # help
    parser.add_argument('TAG_NAME', help="Name of the tag this release note is for. Must be in the form of ^v\d+\.\d+\.\d+$", type=str, nargs=1) # TAG_NAME

    return parser

# Print critical error message and exit
def print_critical_error(message: str, exitCode: int = 1, start: str = "", **args):
    print_error(message, start, **args)
    sys.exit(exitCode)

# Print error message
def print_error(message, start: str = "", **args):
    print(str(start) + "ERROR: " + str(message), file=sys.stderr, flush=True, **args)

# Print warning message
def print_warning(message, start: str = "", **args):
    print(str(start) + "WARNING: " + str(message), flush=True, **args)

# Print info message
def print_info(message, start: str = "", **args):
    print(str(start) + "INFO: " + str(message), flush=True, **args)

# Print debug message
def print_debug(message, start: str = "", **args):
    print(str(start) + "DEBUG: " + str(message), flush=True, **args)
    pass

# +------------------------------------+
# |                                    |
# |             Main Code              |
# |                                    |
# +------------------------------------+

def main() -> int:
    # Parse the arguments
    args = build_arguments_parser().parse_args()
    TAG_NAME: str = args.TAG_NAME[0]

    # Check the git command
    GIT = shutil.which("git")
    if not GIT:
        print_critical_error("The 'git' command is not available! Please install git and try again.")
    
    # Navigate to the repo directory
    os.chdir(REPO_PATH)
    print_info(f"Working Directory: {os.getcwd()!r}")

    # Check if the current directory is a git repository
    if not os.path.exists(".git"):
        print_critical_error("The current directory is not a git repository! Please navigate to a git repository and try again.")

    # Get the repository URL
    repo_url: None | str = None
    try:
        repo_url = str(subprocess.check_output([GIT, "remote", "get-url", "origin"], text=True, stderr=subprocess.DEVNULL)).strip().removesuffix(".git")
    except subprocess.CalledProcessError:
        print_warning("Failed to get the repository URL! Please check the remote URL.")
    print_info(f"Repository URL: {repo_url!r}")

    # Fetch the latest tags
    subprocess.run([GIT, "fetch", "--tags"], check=True)

    # Get the latest tag
    latest_tag: None | str = None
    try:
        latest_tag = str(subprocess.check_output([GIT, "describe", "--tags", "--abbrev=0"], text=True, stderr=subprocess.DEVNULL)).strip()
    except subprocess.CalledProcessError:
        print_warning("No tags found in the repository! Creating release notes from the first commit onwards.")
    print_info(f"Latest Tag: {latest_tag!r}")

    # Check if the latest tag is the same as the current tag
    if latest_tag == TAG_NAME:
        print_critical_error(f"The latest tag is the same as the current tag {TAG_NAME!r}. Skipping the release notes generation.")

    # Current date
    CURRENT_DATE: str = time.strftime("%Y-%m-%d")
    
    # Get the commit logs
    COMMIT_LOGS = [commit.split(" ", 1) for commit in str(subprocess.check_output([GIT, "log", f"{latest_tag}..HEAD" if latest_tag else "HEAD", "--pretty=format:%h %s"], text=True)).strip().split("\n")]
    print_info(f"Parsing {len(COMMIT_LOGS)} commits since last tag...")
    print_debug("\n" + str('\n'.join(["\t" + x[1] for x in COMMIT_LOGS])))

    # Parse the commit logs per category
    release_notes: str = f"# [{TAG_NAME.removeprefix('v')}]({repo_url}/compare/{latest_tag}...{TAG_NAME}) ({CURRENT_DATE})" if latest_tag else f"# {TAG_NAME.removeprefix('v')} ({CURRENT_DATE})"
    release_notes += "\n\n"

    ## Bug Fixes
    fix_commits = [x for x in [(commit[0], BUGFIX_CATEGORY_REGEX.match(commit[1])) for commit in COMMIT_LOGS] if x[1]]
    print_info(f"Found {len(fix_commits)} bug fix commits.")
    if fix_commits:
        print_debug("\n" + str('\n'.join(["\t" + x[1].group(0) for x in fix_commits])))
        release_notes += f"### Bug Fixes\n"
        for commit in fix_commits:
            release_notes += "* "
            if (commit[1].group(1)):
                release_notes += f"**{commit[1].group(1).removeprefix('(').removesuffix(')')}:** "
            release_notes += commit[1].group(2) + f" ([{commit[0]}]({repo_url}/commit/{commit[0]}))" + "\n"

        release_notes += "\n"
    
    ## Features
    feat_commits = [x for x in [(commit[0], FEATURE_CATEGORY_REGEX.match(commit[1])) for commit in COMMIT_LOGS] if x[1]]
    print_info(f"Found {len(feat_commits)} feature commits.")
    if feat_commits:
        print_debug("\n" + str('\n'.join(["\t" + x[1].group(0) for x in feat_commits])))
        release_notes += f"### Features\n"
        for commit in feat_commits:
            release_notes += "* "
            if (commit[1].group(1)):
                release_notes += f"**{commit[1].group(1).removeprefix('(').removesuffix(')')}:** "
            release_notes += commit[1].group(2) + f" ([{commit[0]}]({repo_url}/commit/{commit[0]}))" + "\n"

        release_notes += "\n"
    
    # Check if there are any commits left
    if not fix_commits and not feat_commits:
        print_critical_error("No commits found since the last tag. Skipping the release notes generation.")
    

    # Write the release notes to a file
    release_notes_file = os.path.join(REPO_PATH, "RELEASE_NOTES.md")
    print_info(f"Done. Writing release notes to file {release_notes_file!r}")
    with open(release_notes_file, "w") as f:
        f.write(release_notes)

    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print_critical_error("Interrupted by user.", 130, start="\n")
    # except Exception as e:
    #     printCriticalError(e)