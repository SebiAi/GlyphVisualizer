
# This script is used to update the version and commit hash in the files that are passed as arguments.
# It also writes the version to a file called ".version".
# GITHUB_TOKEN needs to be set as an environment variable because it's needed to run semantic-release.

import os
import subprocess
import sys
import re

def main() -> int:
    # Get the output of a semantic-release dry-run
    output = subprocess.check_output("npx semantic-release --dry-run", shell=True).decode()
    if (os.name == "nt"):
        print(output.encode('cp1252', errors='ignore').decode('cp1252'), end="\n\n\n")
    else:
        print(output, end="\n\n\n")

    # Extract the version from the output
    result = re.findall(r'^\[\d{1,2}:\d{1,2}:\d{1,2} (?:A|P)M\] \[semantic-release\] . .  (?:There is no previous release, t|T)he next release version is (\d+\.\d+\.\d+)$', output, re.MULTILINE)
    if len(result) == 0:
        print("Could not find new version in semantic-release output, trying to extract the old version instead")
        result = re.findall(r'^\[\d{1,2}:\d{1,2}:\d{1,2} (?:A|P)M\] \[semantic-release\] . .  Found git tag v(\d+\.\d+\.\d+) associated with version \d+\.\d+\.\d+ on branch .+$', output, re.MULTILINE)
        if (len(result)) == 0:
            print("Could not find the old version => Using default")
    if len(result) > 1:
        print("Error: Found multiple versions in semantic-release output")
        return 1
    version = "v" + str(result[0]) if len(result) > 0 else "0.0.0"

    # Write the version to a file
    with open(".version", 'w') as f:
        f.write(version)

    # Extract the commit hash from the output
    commit = subprocess.check_output("git rev-parse HEAD", shell=True).strip().decode()

    # Loop trough all the passed arguments (files) and replace the version and commit hash in them
    for i in range(1, len(sys.argv)):
        # Read the files content
        try:
            with open(sys.argv[i], 'r') as f:
                content = f.read()
        except FileNotFoundError:
            print(f"Could not find file {sys.argv[i]}")
            continue

        # Insert the version and commit hash
        content = content.replace('VERSION_HERE', version)
        content = content.replace('GIT_COMMIT_HASH_HERE', commit)

        # Write the new content back to the file
        with open(sys.argv[i], 'w') as f:
            f.write(content)
    
    # Return success
    print(f"Updated version to {version} and commit hash to {commit}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
