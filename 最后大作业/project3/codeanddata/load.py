#!/usr/bin/env python3

import rpy2.robjects as robjects

a = [
    'London2013', 'Mumbai2013', 'NewYork2013',
    'SFO2012', 'SFO2013'
]

for n in a:
    robjects.r['load']('data/' + n + '.rda')

    with open('data/' + n + '.csv', 'w') as f:
        for i, j in zip(robjects.r[n][0], robjects.r[n][1]):
            f.write('{}:{}\t{}\n'.format(n, i, j))
