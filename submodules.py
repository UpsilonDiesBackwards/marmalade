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

import argparse
import json
import os.path
import shutil
import subprocess
from os import system, getcwd
import stat

SUBMODULES_FILE = 'submodules.json'
SUBMODULES_LOCK_FILE = 'submodules.lock'

locks = []


def get_submodules():
    with open(SUBMODULES_FILE, 'r') as f:
        submodules = json.load(f)

    return submodules['submodules']


def handle_list(args):
    print('There are {} submodules defined'.format(len(get_submodules())))
    for submodule in get_submodules():
        print('{}'.format(submodule['path']))
        print('\tURL:\t{}'.format(submodule['url']))
        if 'branch' in submodule:
            print('\tBranch:\t{}'.format(submodule['branch']))
        if 'commit' in submodule:
            print('\tCommit\t{}'.format(submodule['commit']))
        if 'patches' in submodule:
            print('\tPatches:')
            for patch in submodule['patches']:
                print('\t\t{}'.format(patch))


def handle_add(args):
    new_submodule = {
        'path': args.path,
        'url': args.url,
    }

    with open(SUBMODULES_FILE, 'r+') as f:
        submodules = json.load(f)

        if any(submodule['path'] == args.path for submodule in submodules['submodules']):
            print('{} already exists'.format(args.path))
            return

        submodules['submodules'].append(new_submodule)

        f.seek(0)
        json.dump(submodules, f, indent=2)
        f.truncate()

    read_lock_file()
    restore_submodule(new_submodule)
    write_lock_file()


def remove_readonly(func, path, excinfo):
    if excinfo[0] == PermissionError:
        os.chmod(path, 0o777)
        func(path)


def handle_delete(args):
    with open(SUBMODULES_FILE, 'r+') as f:
        submodules = json.load(f)

        found = False
        for i, submodule in enumerate(submodules['submodules']):
            if submodule['path'] == args.path:
                submodules['submodules'].pop(i)
                found = True
                break

        if not found:
            print('{} does not exist'.format(args.path))

        f.seek(0)
        json.dump(submodules, f, indent=2)
        f.truncate()

    shutil.rmtree(args.path, onerror=remove_readonly)
    read_lock_file()
    for lock in locks:
        if args.path in lock:
            locks.remove(lock)

    write_lock_file()


def handle_remove_readonly(func, path, exc):
    excvalue = exc[1]
    if func in (os.rmdir, os.remove, os.unlink) and excvalue.errno == 13:
        os.chmod(path, stat.S_IWRITE)
        func(path)
    else:
        raise

def restore_submodule(submodule, update=False):
    path = submodule['path']
    url = submodule['url']

    if update and os.path.exists(path):
        print("Updating existing submodule at {}".format(path))
        try:
            subprocess.run(['git', '-C', path, 'fetch'], check=True)
            subprocess.run(
                ['git', '-C', path, 'reset', '--hard', 'origin/{}'.format(submodule.get('branch', 'main'))],
                check=True)
        except subprocess.CalledProcessError:
            print("Failed to update {}, recloning instead".format(path))
            shutil.rmtree(path, onerror=handle_remove_readonly)

    if not update or (update and not os.path.exists(path)):
        print("Restoring {} to {}".format(url, path))
        if 'branch' in submodule:
            system('git clone --branch {} {} {}'.format(submodule['branch'], url, path))
        else:
            system('git clone {} {}'.format(url, path))

    if 'commit' in submodule:
        subprocess.run(['git', 'checkout', submodule['commit']], cwd=path)

    patched = ''
    if 'patches' in submodule:
        for patch in submodule['patches']:
            print('Applying patch {}'.format(patch))
            subprocess.run(['git', 'apply', os.path.join(getcwd(), patch)], cwd=path)
            patched = ' *'

    commit_hash = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=path).strip().decode('utf-8')
    branch = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD'],
                                     cwd=path).strip().decode('utf-8')
    if branch == 'HEAD':
        branch = 'detached'
    locks.append('{} {} ({}){}'.format(commit_hash, path, branch, patched))


def handle_restore(args):
    submodules = get_submodules()
    print('Restoring {} submodules'.format(len(submodules)))
    for submodule in submodules:
        restore_submodule(submodule)
    write_lock_file()

def get_submodule_by_path(path):
    submodules = get_submodules()
    for sub in submodules:
        if sub['path'] == path:
            return sub
    return None

def handle_sync(args):
    read_lock_file()
    for entry in locks:
        parts = entry.split(' ')
        commit = parts[0]
        path = parts[1]
        branch = parts[2].strip('()')
        patches_applied = parts[-1] == '*'

        if not os.path.exists(path):
            submodule = get_submodule_by_path(path)
            if submodule is None:
                print("Submodule {} not found in JSON, skipping".format(path))
                continue
            restore_submodule(submodule, update=True)

        try:
            subprocess.run(['git', 'reset', '--hard'], cwd=path, check=True)
            subprocess.run(['git', 'checkout', branch], cwd=path, check=True)
            subprocess.run(['git', 'checkout', commit], cwd=path, check=True)
        except subprocess.CalledProcessError as e:
            print("Failed to sync {}: {}".format(path, e))
            continue


        if patches_applied:
            submodule = get_submodule_by_path(path)
            if submodule and 'patches' in submodule:
                for patch in submodule['patches']:
                    patch_path = os.path.join(os.getcwd(), patch)
                    print("Reapplying patch {} to {}".format(patch_path, path))
                    subprocess.run(['git', 'apply', patch_path], cwd=path, check=False)

    print("Sync complete!")

def handle_update(args):
    submodules = get_submodules()
    print('Restoring {} submodules'.format(len(submodules)))
    for submodule in submodules:
        restore_submodule(submodule, True)
    write_lock_file()


def read_lock_file():
    global locks
    locks.clear()
    with open(SUBMODULES_LOCK_FILE, 'r') as f:
        locks = f.readlines()

    locks = [line.strip() for line in locks]


def write_lock_file():
    with open(SUBMODULES_LOCK_FILE, 'w') as f:
        f.seek(0)
        for lock in locks:
            f.write(lock + '\n')
        f.truncate()


def main():
    parser = argparse.ArgumentParser(description='Marmalade submodules tool')
    subparsers = parser.add_subparsers(dest='command', required=True)

    parser_list = subparsers.add_parser('list', help='List submodules')
    parser_list.set_defaults(func=handle_list)

    parser_add = subparsers.add_parser('add', help='Add submodule')
    parser_add.add_argument('url', help='Git repository URL')
    parser_add.add_argument('path', help='Local path to store the submodule')
    parser_add.set_defaults(func=handle_add)

    parser_add = subparsers.add_parser('delete', help='Delete submodule')
    parser_add.add_argument('path', help='Local path of the submodule')
    parser_add.set_defaults(func=handle_delete)

    parser_restore = subparsers.add_parser('restore', help='Restore submodules for project')
    parser_restore.set_defaults(func=handle_restore)

    parser_sync = subparsers.add_parser('sync', help='Sync submodules with lock file')
    parser_sync.set_defaults(func=handle_sync)

    parser_update = subparsers.add_parser('update', help='Update submodules to latest remote versions')
    parser_update.set_defaults(func=handle_update)

    args = parser.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
