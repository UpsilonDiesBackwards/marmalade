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

import json
import os
import sys
import shutil
import re

includes = {}

def extract_param_names(arg_string):
    args = [arg.strip() for arg in arg_string.split(',') if arg.strip()]
    names = []
    for arg in args:
        arg = arg.split('=')[0].strip()
        parts = arg.split()
        if parts:
            name = parts[-1]
            name = name.lstrip('*&')
            names.append(name)
    return ', '.join(names)

with open("imgui_hooks.json", 'r') as f:
    hooks = json.load(f)
    for hook in hooks:
        print('Adding hook for {}'.format(hook['file']))

        patched_file = os.path.join(sys.argv[2], hook['file'])
        if not os.path.exists(patched_file):
            shutil.copyfile(os.path.join(sys.argv[1], hook['file']), patched_file)

        pattern = re.compile(
            r'([a-zA-Z_][\w:<>\s*&]+?)\s+' + hook['function'] + r'\s*\((.*?)\)\s*{',
            re.DOTALL
        )

        with open(patched_file, 'r+') as f2:
            def replacer(match):
                args = match.group(2)
                param_names = extract_param_names(args)

                call = hook['callback'] + '('
                if hook['add_this']:
                    call += 'this, '
                call += param_names + ')'

                return f"{match.group(0)}\n    if ({call}) return;"

            patched_code, count = pattern.subn(replacer, f2.read())

            f2.seek(0)
            f2.write(patched_code)
            f2.truncate()

        includes.setdefault(hook['file'], []).append(hook['include_file'])

# Process includes
for filename, file_includes in includes.items():
    patched_file = os.path.join(sys.argv[2], filename)
    with open(patched_file, 'r+') as f:
        lines = f.readlines()
        for i, line in enumerate(lines):
            if line.strip() == '#include "imgui.h"':
                insert_pos = i + 1
                break
        else:
            insert_pos = 0

        lines[insert_pos:insert_pos] = [
            f'#include "{inc}"\n' for inc in file_includes
        ]

        f.seek(0)
        f.writelines(lines)
        f.truncate()
