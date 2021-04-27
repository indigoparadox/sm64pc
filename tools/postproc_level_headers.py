#!/usr/bin/env python3

import sys
with open( sys.argv[1], 'r' ) as in_file:
    with open( sys.argv[2], 'w' ) as out_file:
        while True:
            line = in_file.readline()

            if not line:
                break

            if line.startswith( '#' ):
                continue

            if line.strip():
                out_file.write( '#include "{}"\n'.format( line.strip() ) )
