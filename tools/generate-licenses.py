#  Marmalade - Lightweight Game Engine
#  Copyright (C) 2025 Tayler Parsons
#  Copyright (C) 2025 Ryan Bester
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

# generate-licenses.py
#
# Simple tool to generate an open source licenses JSON file for display in
# the about dialog.

import os
import json

VENDOR_DIR = '../vendor'
OUTPUT_FILE = '../res/packages.json'
LICENSE_FILES = ['LICENSE', 'LICENSE.txt', 'LICENCE.txt', 'LICENSE.md', 'LICENSE.MIT', 'COPYING', 'COPYING.txt']


def find_license(package_path):
    for license_file in LICENSE_FILES:
        for variant in [license_file, license_file.lower(), license_file.upper()]:
            license_path = os.path.join(package_path, variant)
            if os.path.isfile(license_path):
                with open(license_path, 'r', encoding='utf-8') as f:
                    return f.read().strip()
    return 'Unknown'


def main():
    packages = []
    if not os.path.exists(VENDOR_DIR):
        print(f'Error: "{VENDOR_DIR}" directory not found.')
        return

    for package_name in os.listdir(VENDOR_DIR):
        package_path = os.path.join(VENDOR_DIR, package_name)
        if os.path.isdir(package_path):
            license_text = find_license(package_path)
            packages.append({'name': package_name, 'license': license_text})

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        json.dump({'packages': packages}, f, indent=4)

    print(f'Generated "{OUTPUT_FILE}" with {len(packages)} packages.')


if __name__ == '__main__':
    main()
