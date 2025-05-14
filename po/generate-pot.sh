#!/usr/bin/env bash

#
# Marmalade - Lightweight Game Engine
# Copyright (C) 2025 Tayler Parsons
# Copyright (C) 2025 Ryan Bester
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

POT_FILE="marmalade.pot"
LINES_TO_STRIP="18"
DATE=$(date "+%Y-%m-%d %H:%M%z")
TMPFILE="$(mktemp)"

find ../src \( -name "*.cpp" -o -name "*.h" \) > POTFILES
xgettext --add-comments=Translators --keyword=_ -o $POT_FILE --files-from=POTFILES


read -r -d '' HEADER << EOM
# Marmalade - Lightweight Game Engine
# Copyright (C) 2025 Tayler Parsons
# Copyright (C) 2025 Ryan Bester
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#, fuzzy
msgid ""
msgstr ""
"Project-Id-Version: Marmalade 0.0.1\\n"
"Report-Msgid-Bugs-To: \\n"
"POT-Creation-Date: $DATE\\n"
"PO-Revision-Date: $DATE\\n"
"Last-Translator: \\n"
"Language-Team: \\n"
"Language: \\n"
"MIME-Version: 1.0\\n"
"Content-Type: text/plain; charset=CHARSET\\n"
"Content-Transfer-Encoding: 8bit\\n"
EOM

tail -n +$((LINES_TO_STRIP + 1)) "$POT_FILE" > "$TMPFILE.body"

echo "$HEADER" > "$TMPFILE"
cat "$TMPFILE.body" >> "$TMPFILE"

mv "$TMPFILE" "$POT_FILE"
rm -f "$TMPFILE.body"

echo "POT file generated"
