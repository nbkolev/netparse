#!/bin/bash

# output infinite stream of expressions and count lines ingested with `pv -l`
bash infinitestream.sh |pv -l | nc localhost 1234 >/dev/null
