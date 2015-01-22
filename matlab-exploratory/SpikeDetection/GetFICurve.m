function FI = GetFICurve(varargin)
% FI = GetFICurve(FileName, CellName, plotRequested, options.tCutoff)
%        ---OR---
% FI = GetFICurve(t, v, I, plotRequested, options.tCutoff)
%
% if no parameters are passed, a structure filled with NaNs is returned.
%  INPUTS:
%   -t is time in ms
%   -v is voltage in mV
%   -I is current in nA
%   -FileName is the name of an .abf file with FI data
%   -CellName is the name of the requested cell within the
%      file specified by FileName (e.g. "top" or "bot")
%    OPTIONAL:
%     -tCutoff: sets time (in s) during current injection to look at spikes
%        if options.tCutoff > 0, looks at first options.tCutoff seconds of
%          data
%        if options.tCutoff < 0, looks at last abs(options.tCutoff) seconds
%          of data
%        otherwise looks at everything
%        (defaults to 0.0)
%     -settleTime: ignore first settleTime seconds of data after onset of
%        current injection.  defaults to 3.0
%     -plotRequested: set to true/1 to plot the resulting FI curve, and
%        potentially make additional requested plots (defaults to false/0)
%     -overallSpikePlotRequested: if plotRequested, make a plot showing
%        the raw traces and detected spikes (default to true/1)
%     -indivSpikePlotRequested:  if plotRequested, make a plot for each
%        individual trace showing spike detection (from withing GetSpikes)
%        this is probably not useful.  (default to false/0)
%  OUTPUTS:
%   -FI: structure with information related to F-I curve
%     -FI.F is list of frequencies in Hz
%     -FI.I is list of currents in nA
%     -FI.Slope is slope of non-zero part of FI curve
%     -FI.Offset is offset of non-zero part of FI curve
%     -FI.Thresh_I is estimated spike threshold current
%     -FI.spike is list of spike analysis structures for each trace
%    The remaining are value/voltage pairs for the locations:
%      -PreMaxCurve = maximum curvature before spike
%      -Deriv = maximum derivative
%      -SpikeHeight and SpikeWidth (hopefully obvious)

% set the default options
defaultOptions = ...
  struct('fileName', '', 'cellName', '', ...
         'tCutoff', 0.0, 'settleTime', 3.0, ...
         'plotRequested', false, 'overallSpikePlotRequested', true, ...
         'indivSpikePlotRequested', false);
% use supplied data/options and default options to get voltage/current
% traces       
[t, v, I, okay, options] = getTraces(defaultOptions, varargin{:});
if ~okay
  % No data, so return an empty structure
  FI = struct('F', [], 'I', [], ...
              'Slope', NaN, 'Offset', NaN, 'Thresh_I', NaN, ...
              'spike', [], ...
              'PreMaxCurve', struct('K', [], 'V', []), ...
              'Deriv', struct('D', [], 'V', []), ...
              'SpikeHeight', struct('H', [], 'V', []), ...
              'SpikeWidth', struct('W', [], 'V', []));
  return
end

% get the FI structure
FI = getFI(t, v, I, options);

if options.plotRequested
  % plot the results
  plotFICurve(t, v, I, options, FI);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [t, v, I, okay, options] = getTraces(defaultOptions, varargin)
if ischar(varargin{1})
  % supplied a file name, load in the data from there
  options = GetOptions(defaultOptions, varargin);
  if isempty(options.fileName)
    t = [];
    v = [];
    I = [];
    okay = false;
  else
    [t, v, I, okay] = ...
      LoadCleanIntrinsicData(options.fileName, options.cellName);
  end
else
  % supplied a trace
  options = GetOptions(defaultOptions, varargin{4:end});
  t = varargin{1};
  v = varargin{2};
  I = varargin{3};
  okay = true;
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [onInd, offInd] = getCurrentPulseInds(t, I, options)
%First find when the current turns on and off

% find on and off times when current is >= half its maximum value
maxI = max(I(:,end));
onInd = find(I(:,end) > 0.5 * maxI, 1) + 1;
offInd = find(I(:,end) > 0.5 * maxI, 1, 'last') - 1;
deltaT = (t(2) - t(1)) / 1000;  % convert to seconds

% if there's a time cutoff, adjust the on and off times to reflect it
if options.tCutoff > 0
  % set offInd to ignore times after tCutoff
  offInd = round(options.tCutoff / deltaT);
elseif options.tCutoff < 0
  % set onInd to ignore times before end + tCutoff
  onInd = round(length(t) + options.tCutoff / deltaT);
end

% if there's a settle time, adjust the on time to reflect it
if options.settleTime > 0
  onInd = onInd + round(options.settleTime / deltaT);
end
%fprintf('on ind: %d, off ind: %d\n', [onInd, offInd])
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function FI = getFI(t, v, I, options)
%First find when the current turns on and off
[onInd, offInd] = getCurrentPulseInds(t, I, options);

NumTraces = size(v,2);
STimes = cell(NumTraces,1);
SMaxes = cell(NumTraces,1);
FI_F = [];
FI_I = [];
FI_K_K = [];
FI_K_V = [];
FI_D_D = [];
FI_D_V = [];
FI_SH_H = [];
FI_SH_V = [];
FI_SW_W = [];
FI_SW_V = [];
FI_Spike = [];
for n = 1:NumTraces
  spike = GetSpikes(t(onInd:offInd), v(onInd:offInd,n), ...
                    'plotSubject', options.indivSpikePlotRequested);

                  
  
  STimes{n} = spike.times;
  SMaxes{n} = spike.maxV.v;
  Num = length(STimes{n});
  if Num == 0
    Freq = 0;
  elseif Num == 1
    Freq = 0; %spike.freq;
  else
    Freq = 1000 * (Num - 1) / (STimes{n}(end) - STimes{n}(1));
  end
  FI_F = [FI_F, Freq]; %#ok<AGROW>
  FI_I = [FI_I, mean(I(onInd:offInd,n))]; %#ok<AGROW>
  [FI_K_K, FI_K_V] = getMeanFIProps(FI_K_K, FI_K_V, spike, ...
				    {'preMaxCurve', 'K'});
  [FI_D_D, FI_D_V] = getMeanFIProps(FI_D_D, FI_D_V, spike, ...
				    {'maxDeriv', 'dV'});
  [FI_SH_H, FI_SH_V] = getMeanFIProps(FI_SH_H, FI_SH_V, spike, ...
				      {'maxV', 'v'}, ...
				      {'preMaxCurve', 'v'});
  [FI_SW_W, FI_SW_V] = getMeanFIProps(FI_SW_W, FI_SW_V, spike, ...
				      {'postMinV', 't'}, ...
				      {'preMaxCurve', 't'});				       
  FI_Spike = [FI_Spike, spike]; %#ok<AGROW>
end

[slope, offset, threshCurrent, linearInds] = fitLinearRegion(FI_I, FI_F);

FI.F = FI_F;
FI.I = FI_I;
FI.Slope = slope;
FI.Offset = offset;
FI.Thresh_I = threshCurrent;
FI.linearInds = linearInds;
FI.spike = FI_Spike;
FI.PreMaxCurve.K = FI_K_K;
FI.PreMaxCurve.V = FI_K_V;
FI.Deriv.D = FI_D_D;
FI.Deriv.V = FI_D_V;
FI.SpikeHeight.H = FI_SH_H;
FI.SpikeHeight.V = FI_SH_V;
FI.SpikeWidth.W = FI_SW_W;
FI.SpikeWidth.V = FI_SW_V;
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [PropList, VList] = getMeanFIProps(PropList, VList, ...
					   spike, SNames1, SNames2)
if nargin == 4
  TempProp = mean(spike.(SNames1{1}).(SNames1{2}));
  TempV = mean(spike.(SNames1{1}).v);
  PropList = [PropList, TempProp];
  VList = [VList, TempV];
else  %it's a difference between two properties
  if strcmp(SNames1{2}, 't')   %it's a time-difference
    TempProp1 = spike.(SNames1{1}).(SNames1{2});
    TempProp2 = spike.(SNames2{1}).(SNames2{2});
  else                      %it's not a time difference
    TempProp1 = spike.(SNames1{1}).(SNames1{2});
    TempProp2 = spike.(SNames2{1}).(SNames2{2});
  end
  if isempty(TempProp1)  % causes problems with older Matlab versions
    TempProp = NaN;
  else
    TempProp = mean(TempProp1 - TempProp2);
  end
  TempV = mean(spike.preMaxCurve.v);
  PropList = [PropList, TempProp];
  VList = [VList, TempV];
end
return


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [slope, offset, threshCurrent, inds] = fitLinearRegion(FI_I, FI_F)
ind = find(FI_F > 0 & FI_I > 0, 1);
if isempty(ind) || ind > length(FI_F) - 2
  % not enough points to fit a line
  slope = NaN;
  offset = NaN;
  threshCurrent = NaN;
else
  n1 = ind;
  n2 = length(FI_I);
  err = fitSubLine(FI_I, FI_F, n1, n2);
  improved = true;
  while improved
    errIncLow = fitSubLine(FI_I, FI_F, n1 + 1, n2);
    errDecHigh = fitSubLine(FI_I, FI_F, n1, n2 - 1);
    if errIncLow < err && errIncLow <= errDecHigh
      n1 = n1 + 1;
      err = errIncLow;
    elseif errDecHigh < err
      n2 = n2 - 1;
      err = errDecHigh;
    else
      improved = false;
    end
    if n2 - n1 < 4
      improved = false;
    end
  end
  
  coefs = polyfit(FI_I(n1:n2), FI_F(n1:n2), 1);
  slope = coefs(1);
  offset = coefs(2);
  threshCurrent = -offset / slope;
  inds = [n1, n2];
end

return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [err, coefs] = fitSubLine(x, y, n1, n2)
if n2 < n1 + 2
  err = Inf;
  coefs = [NaN, NaN];
  return
end
xFit = x(n1:n2);
yFit = y(n1:n2);
coefs = polyfit(xFit, yFit, 1);
residuals = yFit - (coefs(1) * xFit + coefs(2));

err = residuals * residuals' / (length(xFit) - 2);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function plotFICurve(t, v, I, options, FI) %#ok<INUSL>
ind = strfind(options.fileName, filesep);
if ~isempty(ind)
  titleStr = options.fileName((ind(end)+1):end);
else
  titleStr = options.fileName;
end

h = NamedFigure(titleStr);
set(h, 'WindowStyle', 'docked');
hold off
plot(0.001 * t, v)
if options.overallSpikePlotRequested
  hold on
  numTraces = size(v, 2);
  for n = 1:numTraces
    spike = FI.spike(n);
    plot(0.001 * spike.times, spike.maxV.v, 'k.', 'MarkerSize', 5)
  end
  hold off
end
xlabel('Time (s)', 'FontSize', 18, 'FontName', 'Arial')
ylabel('Voltage (mV)', 'FontSize', 18, 'FontName', 'Arial')
title(RealUnderscores(titleStr), 'FontSize', 18, 'FontName', 'Arial')

titleStr = ['FI plot for ', titleStr];
h = NamedFigure(titleStr);
set(h, 'WindowStyle', 'docked');
hold off
plot(FI.I, FI.F, 'rx', 'MarkerSize', 10)
hold on
plot(FI.I, FI.Offset + FI.Slope * FI.I, 'b-')
hold off
xlabel('Current (nA)', 'FontSize', 18, 'FontName', 'Arial')
ylabel('Frequency (Hz)', 'FontSize', 18, 'FontName', 'Arial')
title(RealUnderscores(titleStr), 'FontSize', 18, 'FontName', 'Arial')
return
