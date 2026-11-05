#!/usr/bin/env python3
"""Record/verify native AfterImage capacity using the unchanged legacy probe.

Requires explicit --name and --fixture, plus --afterimage-max=1..512. All other
arguments are forwarded. The original distribution is copied before changing
its disposable configuration. Native observations are never derived from Go.
"""
import argparse
import json
from pathlib import Path
import re
import sys

import reference_probe as legacy


def main():
    parser = argparse.ArgumentParser(description=__doc__, add_help=False)
    parser.add_argument('--afterimage-max', type=int, required=True)
    limits, forwarded = parser.parse_known_args()
    if not 1 <= limits.afterimage_max <= 512:
        parser.error('AfterImage capacity must be between1 and512')
    recipe = argparse.ArgumentParser(add_help=False)
    recipe.add_argument('--name', required=True)
    recipe.add_argument('--fixture', required=True)
    recipe.add_argument('--record', action='store_true')
    args, _ = recipe.parse_known_args(forwarded)
    if not re.fullmatch('[a-z0-9_-]+', args.name) or not re.fullmatch('[a-z0-9-]+', args.fixture):
        parser.error('Use simple lowercase run/fixture names')
    original_prepare = legacy.prepare
    configured = {}

    def prepare(*parameters, **keywords):
        source = original_prepare(*parameters, **keywords)
        configuration = parameters[1] / 'data/mugen.cfg'
        text, count = re.subn(r'(?im)^\s*AfterImageMax\s*=.*$',
                             'AfterImageMax = ' + str(limits.afterimage_max), configuration.read_text())
        if count != 1:
            raise RuntimeError('Expected one disposable AfterImageMax setting')
        configuration.write_text(text)
        configured['config_sha256'] = legacy.digest(configuration)
        return source

    legacy.prepare = prepare
    sys.argv = [sys.argv[0], *forwarded]
    legacy.main()
    root = Path(__file__).resolve().parents[1]
    evidence_path = root / 'temp/reference-runs' / args.name / 'evidence.json'
    evidence = json.loads(evidence_path.read_text())
    fields = {'effect_limits': {'AfterImageMax': limits.afterimage_max}, **configured}
    if args.record:
        evidence.update(fields)
        evidence_path.write_text(json.dumps(evidence, indent=2) + '\n')
    else:
        expected = json.loads((root / 'tests/testdata/reference' / (args.fixture + '.json')).read_text())
        if any(expected.get(key) != value for key, value in fields.items()):
            raise RuntimeError('AfterImage capacity recipe/configuration differs')
    print('Verified native capacity configuration', limits.afterimage_max, flush=True)


if __name__ == '__main__':
    main()
