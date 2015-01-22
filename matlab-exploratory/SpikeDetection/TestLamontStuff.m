function TestLamontStuff

testDir = '/home/ted/matlab/waveform/LamontDatasets';
extracellularFile = [testDir, '/', '753_079_0036_pdn_extra.abf'];

[t, pdn] = GetExtracellular(extracellularFile, 'pdn');

extra = AnalyzeExtracellular(t, pdn(1,:), 'plotSubject', 'pdn');
extra.burst

return