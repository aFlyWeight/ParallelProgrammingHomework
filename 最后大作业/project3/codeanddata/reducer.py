#!/usr/bin/env python3

import sys
from collections import defaultdict


mins = defaultdict(lambda: 99.9)
maxs = defaultdict(lambda: -99.9)
last_k = None
k = None

for line in sys.stdin:
    if not line.strip():
        continue
    k, v, w = line.strip().split('\t')
    v = float(v)
    w = float(w)

    mins[k] = min(mins[k], v)
    maxs[k] = max(maxs[k], w)

    if last_k != k:
        if last_k:
            print('%s\t%s\t%s' % (last_k, mins[last_k], maxs[last_k]))
        last_k = k

if last_k == k:
    print('%s\t%s\t%s' % (k, mins[last_k], maxs[last_k]))
