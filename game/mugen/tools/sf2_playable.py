#!/usr/bin/env python3
"""Create or update a linked SF2 working copy with missing later-round exits.

The supplied beta stalls too: these legacy state5900 scripts only exit on round1.
Original files stay untouched. Assets are linked; only the matching scripts
and writable configuration/log files are copied. Requires the original pack.
"""
import argparse
from pathlib import Path
import re
import shutil
import tempfile


def repair(source):
    header = re.search(rb'(?im)^\s*\[statedef\s+5900\][^\r\n]*\r?\n', source)
    if header is None:
        return None
    following = re.search(rb'(?im)^\s*\[statedef\s', source[header.end():])
    end = header.end() + following.start() if following else len(source)
    body = source[header.end():end]
    controllers = re.split(rb'(?im)^\s*\[state\b[^\r\n]*\r?\n', body)
    exits = [block for block in controllers
             if re.search(rb'(?im)^\s*type\s*=\s*changestate\s*$', block)]
    if (not re.search(rb'(?im)^\s*trigger1\s*=\s*roundno\s*=\s*1\s*$', body)
            or not exits
            or any(not re.search(rb'(?im)^\s*value\s*=\s*190\s*$', block) for block in exits)):
        return None
    newline = b'\r\n' if b'\r\n' in source else b'\n'
    extra = newline.join([b'', b'[State 5900, later rounds]', b'type = ChangeState',
                          b'trigger1 = RoundNo > 1', b'value = 0', b''])
    return source[:end] + extra + source[end:]


def find_repairs(source):
    patched = {}
    for path in sorted((source / 'chars').rglob('*')):
        if path.is_file() and path.suffix.lower() in {'.cns', '.st', '.cmd'}:
            data = repair(path.read_bytes())
            if data is not None:
                patched[path.relative_to(source)] = data
    if not patched:
        raise ValueError('No matching missing later-round exits found in the source pack')
    return patched


def update_copy(source, output, patched):
    # Preflight every script and linked parent before changing anything. Keep
    # user-edited scripts and configuration intact; only original/our repairs fit.
    for path, data in patched.items():
        destination = output / path
        if destination.read_bytes() not in ((source / path).read_bytes(), data):
            raise ValueError('Working-copy script has local edits: ' + str(path))
        for relative in [*reversed(path.parents)][1:] + [path]:
            target = output / relative
            if target.is_symlink() and target.resolve() != (source / relative).resolve():
                raise ValueError('Working-copy link points to another source: ' + str(relative))
    changed = 0
    for path, data in patched.items():
        if (output / path).read_bytes() == data:
            continue
        for relative in [*reversed(path.parents)][1:]:
            directory = output / relative
            if directory.is_symlink():
                directory.unlink()
                directory.mkdir()
                for child in (source / relative).iterdir():
                    (directory / child.name).symlink_to(child, target_is_directory=child.is_dir())
        destination = output / path
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=destination.parent, delete=False) as file:
                temporary = Path(file.name)
                file.write(data)
            temporary.replace(destination)  # Replaces the script link, never its target.
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
        changed += 1
    return changed


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=root / 'temp/sf2_mugen')
    parser.add_argument('--output', type=Path, default=root / 'temp/sf2_mugen_playable')
    parser.add_argument('--update', action='store_true', help='Extend an existing working copy without replacing configuration or local script edits')
    args = parser.parse_args()
    source, output = args.source.resolve(), args.output.resolve()
    if args.output.is_symlink() or output.is_relative_to(source) or source.is_relative_to(output):
        parser.error('Output must be a separate directory outside the source pack')
    if output.exists() and not args.update:
        parser.error('Output already exists; use --update to extend its repairs')
    if args.update and not output.is_dir():
        parser.error('--update requires an existing working-copy directory')
    try:
        patched = find_repairs(source)
        if args.update:
            changed = update_copy(source, output, patched)
            print('Updated', output, f'({changed} additional repairs; {len(patched)} total)')
            return
    except (OSError, ValueError) as error:
        parser.error(str(error))
    writable = set(patched) | {Path('data/mugen.cfg'), Path('mugen.yaml'), Path('mugen.log')}

    def populate(directory, target, relative=Path()):
        target.mkdir()
        for child in directory.iterdir():
            path = relative / child.name
            destination = target / child.name
            if path in writable:
                if child.is_file():
                    if path in patched:
                        destination.write_bytes(patched[path])
                    else:
                        shutil.copyfile(child, destination)
            elif any(path in item.parents for item in writable):
                populate(child, destination, path)
            else:
                destination.symlink_to(child, target_is_directory=child.is_dir())

    output.parent.mkdir(parents=True, exist_ok=True)
    try:
        populate(source, output)
    except Exception:
        shutil.rmtree(output)
        raise
    print('Prepared', output)
    print(f'{len(patched)} script repairs; original pack unchanged; media assets shared by links.')
    print('Run from the repository root:')
    print('game/mugen/mugen.sh --game --data-dir', output)


if __name__ == '__main__':
    main()
