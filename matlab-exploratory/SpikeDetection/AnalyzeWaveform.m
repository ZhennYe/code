function structOut = AnalyzeWaveform(t, v, varargin)
% StructOut = AnalyzeWaveform(t, v, plotSubject)
% Analyzes a single voltage waveform, looking for spikes
%    and bursts, and calculating relevant frequencies.
%
%  INPUT PARAMETERS:
%   -t is time in ms
%   -v is voltage in mV
%    OPTIONAL:
%     -plotSubject should be set to true[false] to produce[suppress]
%       plots of waveforms/analysis.  Alternatively, it can be set
%       to a string to aid it titling plots (e.g. 'Exp #71')
%       plotSubject defaults to false
%  OUTPUT PARAMETERS:
%   -StructOut.Spike:  structure with spike information
%      -Spike.Freq is overall spiking frequency (in Hz)
%      -Spike.Times is a plain list of spike times (in ms)
%      -Spike.Intervals is a list of interspike intervals (in ms)
%      -Spike.Frequencies is a list of instantaneous frequencies (in Hz)
%      Shape information structures (should be self-descriptive)
%      -Spike.MaxV, Spike.MaxDeriv, Spike.MinDeriv, Spike.PreMinV,
%       Spike.PostMinV, Spike.PreMaxCurve, Spike.PostMaxCurve
%           Each contains a list of times/voltage points, and if relevant
%           another quantity (such as K for curvatures)
%   -StructOut.SlowWave:  structure with slow-wave information
%      -SlowWave.Freq: frequency of the dominant slow-wave
%       component (in Hz)
%      -SlowWave.Sigma: (very crude) measure of the importance
%       of the slow-wave frequency in the power spectrum
%      -SlowWave.Corr: autocorrelation at slow-wave period
%      -SlowWave.Spectrum: structure with spectrum information
%        -Spectrum.Freq:  list of analyzed frequencies
%        -Spectrum.Power: length NumFreq list of average powers of
%           waveform with spikes removed.
%   -StructOut.Burst:  structure with burst information
%      -Burst.Freq is burst frequency (in Hz)
%      -Burst.SpikeFreq is within-burst spike frequency (in Hz)
%      -Burst.DutyCycle is the average burst duration/period
%      -Burst.Times is a plain list of burst times (in ms)
%      -Burst.Durations is a list of burst durations (in ms)
%      -Burst.numSpikes is a list of spikes per burst
%      -Burst.SpikeFrequencies is a list of spike frequencies (in Hz)
%      -Burst.InterBurstIntervals is a list of inter-burst
%       intervals (in ms)
%   -StructOut.MedianV:  the median of the voltage trace.  If the
%      cell is silent, it should be the resting potential,
%      otherwise, who knows...
%
%List structures usually will have a Name.List element, as well as
%  Name.Mean, Name.StdDev, Name.Variance, Name.CoefOfVar
%  (a few are just plain lists)
%If a feature is not detected, relevant frequencies are set to
%  zero, and relevant lists are empty
%
%NOTE for future:  would benefit enormously by changing to .mex
callstack = dbstack;
if length(callstack) == 1  % not called by another function
  tic
end

if nargin < 3
  help AnalyzeWaveform
  error('Invalid number of arguments')
end
if length(t) ~= length(v)
  if length(t) == 1
    dt = t;
    t = 0:dt:(dt * (length(v) - 1));
  else
    error('Time and Voltage arrays have different length!')
  end
end
if size(t, 1) > 1
  t = t';
end
if size(v,2) ~= size(t,2)
  v = v';
end

% set the default options
defaultOptions = { ...
  'plotSubject', false, ...
  'timesOnly', false, ...
  'firstOnly', false, ...
  'lowCutoff', NaN, ...
  'highCutoff', NaN, ...
  'bracketWidth', 5.0, ...
  'pFalseSpike', 1.0e-4, ...
  'debugPlots', false ...
};
% get the options overrides from varargin
options = GetOptions(defaultOptions, varargin);

spike = GetSpikes(t, v, options);

slowWave = AnalyzeSlowWave(t, v, spike, ...
  'plotSubject', options.plotSubject, 'debugPlots', options.debugPlots);
burst = AnalyzeBurst(spike, slowWave, t);
slowWave.Phases = [];  %Reduce storage demand

%structify (add info about mean, variance, etc) various lists
spike.intervals = structifyList(spike.intervals);
spike.frequencies = structifyList(spike.frequencies);

spike.maxV.v = structifyList(spike.maxV.v);
spike.maxDeriv.v = structifyList(spike.maxDeriv.v);
spike.maxDeriv.dV = structifyList(spike.maxDeriv.dV);
spike.minDeriv.v = structifyList(spike.minDeriv.v);
spike.minDeriv.dV = structifyList(spike.minDeriv.dV);
spike.preMinV.v = structifyList(spike.preMinV.v);
spike.postMinV.v = structifyList(spike.postMinV.v);
spike.preMaxCurve.v = structifyList(spike.preMaxCurve.v);
spike.preMaxCurve.K = structifyList(spike.preMaxCurve.K);
spike.postMaxCurve.v = structifyList(spike.postMaxCurve.v);
spike.postMaxCurve.K = structifyList(spike.postMaxCurve.K);

burst.Durations = structifyList(burst.Durations);
burst.SpikesPerBurst = structifyList(burst.SpikesPerBurst);
burst.InterBurstIntervals = structifyList(burst.InterBurstIntervals);
burst.SpikeFrequencies = structifyList(burst.SpikeFrequencies);

structOut.spike = spike;
structOut.SlowWave = slowWave;
structOut.Burst = burst;
structOut.MedianV = median(v);

if needPlot(options)
  dT = t(2) - t(1);
  hSpikes = PlotGetSpikes(dT, v, spike, options, burst);
  
  % link relevant time axis together
  if options.debugPlots
    aSpikes = get(hSpikes, 'CurrentAxes');
    derivsTitle = makeTitle('dV/dT vs. t', options);
    aDerivs = get(findobj('name', derivsTitle),'CurrentAxes');
    kTitle = makeTitle('Curvature', options);
    aK = get(findobj('name', kTitle), 'CurrentAxes');
    aHandles = [aSpikes, aDerivs, aK];
    linkaxes(aHandles, 'x');
  end
end

if length(callstack) == 1  % not called by another function
  Toc
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function outStruct = structifyList(inList)
outStruct.list = inList;
goodInd = find(isfinite(inList));
if length(goodInd) > 1
  inList = inList(goodInd);
  outStruct.mean = mean(inList);
  outStruct.stdDev = std(inList);
  outStruct.variance = outStruct.stdDev^2;
  outStruct.coefOfVar = outStruct.stdDev / outStruct.mean;
else
  if length(goodInd) == 1
    outStruct.mean = inList(goodInd);
  else
    outStruct.mean = 0;
  end
  outStruct.stdDev = 0;
  outStruct.variance = 0;
  outStruct.coefOfVar = 0;
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function plotVar = needPlot(options)
plotVar = ischar(options.plotSubject) || options.plotSubject;
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function titleStr = makeTitle(titleBase, options)
% set the full title for a figure based on base title and plotSubject
if ischar(options.plotSubject)
  titleStr = [options.plotSubject, ': ', titleBase];
else
  titleStr = titleBase;
end
return