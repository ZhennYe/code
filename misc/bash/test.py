string = ['Traceback (most recent call last): \nFile "neuron_readExportedGeometry.py", line 542, in <module>\n  demoRead(geoFile)  File "neuron_readExportedGeometry.py", line 501, in demoRead\n  geometry = HocGeometry(geoFile)\n  File "neuron_readExportedGeometry.py", line 32, in __init__\n  self.readGeometry()\n  File "neuron_readExportedGeometry.py", line 63, in readGeometry\n  self._connectFilaments()\n  File "neuron_readExportedGeometry.py", line 361, in _connectFilaments\n  self._connectSegments(segment0, location0, segment1, location1)\n  File "/home/alex/code/morphology/python/functions/NeuronGeometry.py", line 1221, in _connectSegments\n  (segment0.name, segment1.name)\n  AssertionError: Tried to connect filament_999[9089] and filament_999[9087], which are already connected\n']


##################
if __name__ == '__main__':
  print(string)
