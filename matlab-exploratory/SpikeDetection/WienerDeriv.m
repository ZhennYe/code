function varargout = WienerDeriv(y, dx, varargin)
% [dY, dY2, dY3, ...] = WienerDeriv(y, dx, order, [order2, ...], 'nyquistFraction', fraction)
% Calculate derivative of noisy signal y using the fft of y.
% Assumes noise is white noise and signal is band-limited to have ZERO
%  power at 1/4 the Nyquist frequency or greater.
% INPUTS:
%  -y: true signal plus additive white noise.
%  -dx:  sampling interval
%  OPTIONAL:
%  -order: order of derivative, defaults to 1
%  -order2, etc: can request multiple derivative orders in one call
%  -nyquistFraction: any frequency greater than this fraction of Nyquist
%   frequency is assumed to be noise. Defaults to 0.25
% OUTPUTS:
%  -dY:  estimated derivative of signal
if nargin < 3
  varargin = {1};
  nyquistFraction = 0.25;
elseif nargin < 2
  help WienerDeriv
  if nargout == 0
    return
  else
    error('Invalid number of input arguments.')
  end
else
  nyquistFraction = 0.25;
  n = 1;
  while n <= length(varargin)
    if ischar(varargin{n})
      if strcmpi(varargin{n}, 'nyquistFraction') && n < length(varargin)
        nyquistFraction = varargin{n+1};
        varargin(n:(n+1)) = [];
      else
        help WienerDeriv
        error('Invalid option: %s', varargin{n})
      end
    else
      n = n + 1;
    end
  end
end
  
if size(y,1) > 1
  y = y';
end



% first remove linear trend from y
numY = length(y);
xEnd = dx * (numY - 1);
x = 0:dx:xEnd;
dYAvg = (y(end) - y(1)) / xEnd;
xMean = xEnd / 2;
yAvg = mean(y) + dYAvg * (x - xMean);
y = y - yAvg;

% compute the fft of y, and the spectrum
yFft = fft(y);
[ySpectrum, w] = Spectrum(y, dx, 'removeTrend', false);

% get noise filter: nFilter = 1 - noiseSpectrum ./ ySpectrum
nFilter = getNoiseFilter(ySpectrum, w, nyquistFraction);

% filter out the noise
yFft = yFft .* nFilter;

% compute the derivative for each requested order
varargout = cell(size(varargin));
for n = 1:length(varargin)
  order = varargin{n};
  varargout{n} = getFilteredDeriv(yFft, w, order, yAvg, dYAvg);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function nFilter = getNoiseFilter(ySpectrum, w, nyquistFraction)
% Assume noise is white noise, and signal has ZERO power at 1/4 the
%  Nyquest frequency or faster (up to specified fraction of the Nyquest
%  frequency, to hopefully avoid any aliasing artifacts at the edge of the
%  spectrum)

% Figure out the indices to Nyquist frequency, etc
numCorr = length(ySpectrum);
nyquistFractionInd = round(0.5 * numCorr * nyquistFraction);
wNyquistFraction = abs(w(nyquistFractionInd));

% Anything faster than this is suspect as due to aliasing
aliasInd = round(numCorr * 3 / 8);
% Anything in this range is assumed to be dominated by noise
noiseInds = [nyquistFractionInd:aliasInd, ...
             (numCorr+1-aliasInd):(numCorr+1-nyquistFractionInd)];
yNoise = abs(ySpectrum(noiseInds));

% compute the noise amplitude as the 90th percentile amplitude in noise
% region
yNoise = sort(yNoise);
nAmp = yNoise(round(0.90 * length(yNoise)));

% Set any frequency with amplitude <= nAmp or abs(w) > wNyquistFraction
% as noise
smallNoise = abs(ySpectrum) > nAmp & abs(w) <= wNyquistFraction;

% the noise filter is 1.0 - noiseSpectrum / ySpectrum
nFilter = zeros(size(ySpectrum));
nFilter(smallNoise) = 1.0 - nAmp ./ abs(ySpectrum(smallNoise));
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function dY = getFilteredDeriv(yFft, w, order, yAvg, dYAvg)
% compute the filtered derivative
if order == 0
  dY = ifft(yFft, 'symmetric') + yAvg;
elseif order ~= 0
  yFft = yFft .* (1i * w).^(order);
  dY = ifft(yFft, 'symmetric');
  if order == 1
    dY = dY + dYAvg;
  end
end
return
