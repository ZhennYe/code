% For Anne

% addpath()



function [currents, ydata] = plotFI(traces, currents)
% This will plot an F-I curve when given traces (several) and corresponding
% currents.
%
% 
if nargin < 2
  disp('Need to pass traces AND currents')
end

freqs = zeros(length(traces), 100);
mns = zeros(length(traces),1);
for i=1:length(traces)
  [fqs, spk] = getFreq(traces(i));
  freqs(i) = fqs;
  msn(i) = mean(fqs);
end



% Rudimentary plotting
plot(currents, msn);
end


function [freqs, spk] = getFreq(y, dt=0.2) 
%
%
% Send trace to GetSpikes
spk = GetSpikes(dt, y, false);
freqs = spk.frequencies
end


