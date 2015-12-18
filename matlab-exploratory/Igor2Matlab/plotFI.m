% For Anne


function [currents, means, first] = plotFI(traces, currents)
% This will plot an F-I curve when given traces (several) and corresponding
% currents.
%
% 
if nargin < 2
  disp('Need to pass traces AND currents')
end

[m,n] = size(traces);
if m > n
  numtraces = n;
else
  numtraces = m;
end

freqs = zeros(numtraces, 100);
means = zeros(numtraces,1);
first = zeros(numtraces, 1);
last = zeros(numtraces, 1);

for i=1:numtraces
  i
  try
    spk = GetSpikes(0.2, traces(i,:));
    freqs(i,1:length(spk.frequencies)) = spk.frequencies;
    means(i) = mean(spk.frequencies);
    first(i) = spk.frequencies(1);
    last(i) = spk.frequencies(end);
  catch 
    means(i) = 0;
    first(i) = 0;
    last(i) = 0;
    fprintf('Missed Number %i\n', i)
  end
end


% Rudimentary plotting
figure();
plot(currents, means, 'k');
hold on;
plot(currents, first, 'r');
plot(currents, last, 'g');
end

