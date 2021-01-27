#!/bin/bash

cd opt_cached_ETC/
make
cd build
echo "threebears ETC" > ../../output.txt
./threebears >> ../../output.txt
cd ../..

cd opt_cached
make
cd build
echo "threebears original" >> ../../output.txt
./threebears >> ../../output.txt
cd ../..
