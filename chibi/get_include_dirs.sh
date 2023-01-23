#!/bin/bash
make -n | sed '2q;d' | tr " " "\n" | sed -e 's/^-I//;t;d'

#printf "%s\n" $dirs

