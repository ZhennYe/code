function abfStruct = LoadAbf(fileName, varargin)
% abfStruct = LoadAbf(fileName, WantedString1, WantedString2, ...)
% This script is a wrapper for load_abf from Adam Taylor's
% toolkit. 
% It organizes the data in an .abf file into a structure with
% fields:
%  Time: array of times (in seconds)
%  Header: structure with header information, as from load_abf
%  Data: structure with fields that are arrays, traces (voltage or
%        current) as stored in the .abf file
%  Units: structure with fields that are strings, specifying the
%        units in data.  Data and Units have the same field names.


[t, data, h] = load_abf(fileName);
numChan = h.nADCNumChannels;
if size(data, 2) ~= numChan
  data = permute(data, [2 1 3]);
end

abfStruct.Time = t;
abfStruct.Header = h;
numFound = 0;
for m = 1:numChan
  sampleNum = h.nADCSamplingSeq(m) + 1;
  fieldName = deblank(h.sADCChannelName(sampleNum,:));
  
  units = deblank(h.sADCUnits(sampleNum,:));
  fieldData = squeeze(data(:,m,:));

  if nargin > 1
    notWanted = true;
    for n = 1:length(varargin)
      if ~isempty(strfind(fieldName, varargin{n}))
	notWanted = false;
	break
      end
    end
    if notWanted
      continue
    end
  end
  
  numFound = numFound + 1;
  fieldName = replaceSpaces(fieldName);
  abfStruct.Data.(fieldName) = fieldData;
  abfStruct.Units.(fieldName) = units;
end

if numFound == 0
  error('Unable to find requested wave data')
end

return


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function fieldName = replaceSpaces(fieldName)
ind = strfind(fieldName, ' ');
fieldName(ind) = '_';
return
