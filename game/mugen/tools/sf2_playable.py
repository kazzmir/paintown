#!/usr/bin/env python3
"""Create a small SF2 working copy with Ryu/Dhalsim's missing round-two exits.

The supplied beta stalls too: these legacy state5900 scripts only exit on round1.
Original files stay untouched. Assets are linked; only the two repaired scripts
and writable configuration/log files are copied. Requires the original pack.
"""
import argparse
from pathlib import Path
import re
import shutil


SCRIPTS = [Path('chars/sf2 Ken/SF2-KEN/char/common.st'),
           Path('chars/Sf2 Dhalsim/SF2-DHALSIM/char/common.st')]


def repair(source):
    header = re.search(rb'(?im)^\s*\[statedef\s+5900\][^\r\n]*\r?\n', source)
    if header is None:
        raise ValueError('Missing state5900')
    following = re.search(rb'(?im)^\s*\[statedef\s', source[header.end():])
    end = header.end() + following.start() if following else len(source)
    body = source[header.end():end]
    controllers = re.split(rb'(?im)^\s*\[state\b[^\r\n]*\r?\n', body)
    exits = [block for block in controllers
             if re.search(rb'(?im)^\s*type\s*=\s*changestate\s*$', block)]
    if (not re.search(rb'(?im)^\s*trigger1\s*=\s*roundno\s*=\s*1\s*$', body)
            or not any(re.search(rb'(?im)^\s*value\s*=\s*190\s*$', block) for block in exits)
            or any(re.search(rb'(?im)^\s*value\s*=\s*0\s*$', block) for block in exits)):
        raise ValueError('State5900 does not match the observed missing later-round exit')
    newline = b'\r\n' if b'\r\n' in source else b'\n'
    extra = newline.join([b'', b'[State 5900, later rounds]', b'type = ChangeState',
                          b'trigger1 = RoundNo > 1', b'value = 0', b''])
    return source[:end] + extra + source[end:]


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=root / 'temp/sf2_mugen')
    parser.add_argument('--output', type=Path, default=root / 'temp/sf2_mugen_playable')
    args = parser.parse_args()
    source, output = args.source.resolve(), args.output.resolve()
    if output.exists() or output.is_symlink() or output.is_relative_to(source):
        parser.error('Output must be a fresh directory outside the source pack')
    try:
        patched = {path: repair((source / path).read_bytes()) for path in SCRIPTS}
    except (OSError, ValueError) as error:
        parser.error(str(error))
    writable = set(SCRIPTS + [Path('data/mugen.cfg'), Path('mugen.yaml'), Path('mugen.log')])

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
    print('Two script repairs; original pack unchanged; media assets shared by links.')
    print('Run from the repository root:')
    print('game/mugen/mugen.sh --game --data-dir', output)


if __name__ == '__main__':
    main()
