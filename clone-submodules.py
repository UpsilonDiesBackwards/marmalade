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

# clone-submodules.py
#
# Parses the .gitmodules file and clones each submodule

import configparser
from os import system

SUBMODULES_FILE = '.gitmodules'

def main():
    c = configparser.ConfigParser()
    c.read(SUBMODULES_FILE)

    mods = {}
    for section in c.sections():
        if section.startswith('submodule'):
            name = section.split('"')[1]
            path = c.get(section, 'path', fallback=None)
            url = c.get(section, 'url', fallback=None)
            mods[name] = {'path': path, 'url': url}

    print('Cloning {} submodules...'.format(len(mods)))

    for _, mod in mods.items():
        print('Cloning {} into {}'.format(mod['url'], mod['path']))
        system('git clone {} {}'.format(mod['url'], mod['path']))

if __name__ == '__main__':
    main()
