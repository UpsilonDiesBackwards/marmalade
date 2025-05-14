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

# pseudo-localise.py
#
# Generates pseudo localisations from po file

import os
from polib import pofile

POT_FILE = '../po/marmalade.pot'
OUTPUT_FILE = "../po/en@pseudo.po"


def pseudo_localize(s):
    table = str.maketrans("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
                          "àƀçďëƒĝħïĵķľɱñôƥɋřşţüṽŵẋÿžÀƁÇĎËƑĜĦÏĴĶĽɱÑÔƤɊŘŞŦÜṼŴẊŸŽ")
    s = s.translate(table)
    return f"[!! {s} !!]"


def main():
    po = pofile(POT_FILE)

    for entry in po:
        if entry.msgid_plural:
            for idx in entry.msgstr_plural:
                text = entry.msgid if idx == 0 else entry.msgid_plural
                entry.msgstr_plural[idx] = pseudo_localize(text)
        else:
            entry.msgstr = pseudo_localize(entry.msgid)

    po.metadata['Content-Type'] = 'text/plain; charset=UTF-8'
    po.metadata['Content-Transfer-Encoding'] = '8bit'
    po.metadata['Plural-Forms'] = 'nplurals=2; plural=(n != 1);'
    po.charset = 'UTF-8'

    po.save(OUTPUT_FILE)
    print('Done')


if __name__ == '__main__':
    main()
