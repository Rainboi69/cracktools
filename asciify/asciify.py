#!/usr/bin/env python3

import sys

if len(sys.argv) != 2:
    print("ERROR: No file specified", file=sys.stderr)
    exit(1)

with open(sys.argv[1], 'rb') as f:
    print('"{}"'.format("{}".format(f.read())[2:-1]))

