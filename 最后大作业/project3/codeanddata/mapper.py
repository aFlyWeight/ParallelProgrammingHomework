#!/usr/bin/env python3

import sys

for line in sys.stdin:
    line = line.strip()
    if not line:
        continue
    k, v = line.split('\t', 1)
    try:
        k = k.split(' ', 1)[0]
        v = v
    except ValueError:
        continue
    print('%s\t%s\t%s' % (k, v, v))
