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

import sys

# Check the python version
if sys.version_info < (3, 10):
    print("This script requires Python 3.10 or higher! Please upgrade your python version and try again.")
    sys.exit(1)

import os
import subprocess
import shutil

# +------------------------------------+
# |                                    |
# |              Globals               |
# |                                    |
# +------------------------------------+

# Get the script directory
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

# Relative path to the repository
REPO_PATH = os.path.join(SCRIPT_DIR, '../')

# License text
LICENSE_TEXT = """
This file is part of the GlyphVisualizer project, a Glyph composition
player that plays Glyph compositions from Nothing phones.
Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
""".strip()

# Extensions to check and the possible comments used
EXTENSIONS: dict[str, list[str]] = {
    'c': ['/*', '*/'],
    'cpp': ['/*', '*/'],
    'h': ['/*', '*/'],
    'hpp': ['/*', '*/'],
    'py': ['#'],
    'sh': ['#'],
    'bat': ['REM', 'rem', '::'],
}

# +------------------------------------+
# |                                    |
# |           Bioler Plate             |
# |                                    |
# +------------------------------------+

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
    #print(str(start) + "DEBUG: " + str(message), flush=True, **args)
    pass

# +------------------------------------+
# |                                    |
# |             Functions              |
# |                                    |
# +------------------------------------+

def get_extension(file_path: str) -> str | None:
    # Get the extension
    split = os.path.splitext(file_path)
    # Skip files without an extension
    if not split[1]:
        return None
    # Handle special CMake input files
    if split[1][1:] == "in":
        # Get the real extension
        split = os.path.splitext(split[0])
        # Skip files without an extension
        if not split[1]:
            return None
    # Get the extension
    return split[1][1:]

# +------------------------------------+
# |                                    |
# |             Main Code              |
# |                                    |
# +------------------------------------+

def main() -> int:
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
    
    # Get a list of all files in the repository
    try:
        file_paths = str(subprocess.check_output([GIT, "ls-files", "--cached", "--others", "--exclude-standard"], text=True)).strip().splitlines()
    except subprocess.CalledProcessError:
        print_critical_error("Failed to get the list of files in the repository!")
    print_info(f"Found {len(file_paths)} files in the repository.\n")
    
    # Check each file for the license disclaimer
    missing_disclaimer_count = 0
    recognized_files = 0
    for file_path in file_paths:
        # Get the extension
        ext = get_extension(file_path)
        if not ext:
            continue
        # Check if the extension is supported
        if ext not in EXTENSIONS:
            print_debug(f"Unsupported file extension: {file_path!r}")
            continue
        recognized_files += 1
        # Get the comment strings
        comments = EXTENSIONS[ext]

        # Read the file
        with open(file_path, 'r') as file:
            file_lines = file.readlines()
        
        # Clean out the comments and whitespace per line
        cleaned_lines = []
        for line in file_lines:
            for c in comments:
                line = line.replace(c, "")
            cleaned_lines.append(line.strip())

        first_license_line = LICENSE_TEXT.splitlines()[0]
        # Check each line for the license disclaimer
        # Must be at the start of the file
        shebang_encountered = False
        for i, line in enumerate(cleaned_lines):
            # Skip empty lines
            if not line.strip():
                continue
            # Check if the license text is present
            if first_license_line != line:
                # Ignore lines starting with '!' if we have a script file
                # This is a workaround for the shebang line in script files
                if ext in ['py', 'sh'] and line.startswith('!') and not shebang_encountered:
                    shebang_encountered = True
                    continue
                # Ignore "@echo off" in batch files
                if ext == 'bat' and line == '@echo off':
                    continue
                # License disclaimer not found at the start of the file
                # Exit if, the next if statement will not be true => license disclaimer not found
            # Check if the license text is present
            if LICENSE_TEXT == '\n'.join(cleaned_lines[i:i+len(LICENSE_TEXT.splitlines())]):
                print_debug(f"License disclaimer found in file: {file_path!r}:{i+1}")
                break
            # License disclaimer not found at the start of the file
            missing_disclaimer_count += 1
            print_warning(f"License disclaimer not found in file: {file_path!r}")
            break
    
    # Print the summary
    print_info(f"Missing license disclaimer in {missing_disclaimer_count}/{recognized_files} recognized files.", start="\n")

    return 0

if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print_critical_error("Interrupted by user.", 130, start="\n")
    # except Exception as e:
    #     printCriticalError(e)