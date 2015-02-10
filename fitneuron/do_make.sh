#!/bin/bash
# first update the old version information in src/fitneuron.cpp
oldVersionInfo=$(grep "fitneuron version" src/fitneuron.cpp | sed 's/\([[\."$]\)/\\\1/g')
versionInfo="cout << \"$(head -n 1 VERSION.txt)\\\n\";"
sed -i "s/${oldVersionInfo}/${versionInfo}/" src/fitneuron.cpp

# next create a new Makefile
./python/CreateMakefile.py simulate_neuron.bin fitneuron.bin perturbneuron.bin -shared injectors

# finally make
make
