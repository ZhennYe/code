function TestJonStuff

TestDir = '/home/ted/matlab/waveform/JonDatasets';

dirList = dir(TestDir);
for n = 1:length(dirList)
  name_n = dirList(n).name;
  if(strncmp(name_n, '.', 1))
    continue;
  end
  disp(name_n)
  clear t v;
  load([TestDir, '/', name_n]);
  An = AnalyzeWaveform(t, v, name_n);
end


return