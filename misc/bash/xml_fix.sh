# bash script to automate removal of loops from newly created hoc files

problems = 1
while [problems -eq 1]
do
outpython = $(python ~/code/morphology/python/functions/neuron_readExportedGeometry.py ~/code/morphology/python/functions/temp_hoc.hoc)

