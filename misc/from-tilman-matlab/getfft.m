% wrapper to compute fft and relevant related quantities
% parameters
% d: data in time domain
% fs: sampling frequency in Hz
% vararg: [a b] two element vector defining frequency range to focus on
function ff = getfft(d, fs, varargin)

% focus on range 0 - 100 Hz if not otherwise specified
if (nargin == 2)
    flim = [0 100];
    dfft = 1;
elseif (nargin == 3)
    flim = varargin{1};
    dfft = 1;
else
    flim = varargin{1};
    dfft = varargin{2};
end

% first, detrend, demean
d = detrend(d);
d = d - mean(d);

% compute length of the FFT, and frequency values
nfft = round(numel(d)/dfft);
% frequency resolution per DFT bin
df = fs / nfft; 
% from Mike / stackoverflow
% supposedly, calculating the freq axis this way will account for even or
% odd length. Doing this will get you to -fs/2 to fs/2 (negative and
% positive Nyquist frequencies)
% alternatively, this would be only positive frequencies, all the way up to fs
% myfreq = (0:df:(fs-df)); 
myfreq = (0:df:(fs-df)) - (fs-mod(nfft,2)*df)/2;
myfreq = myfreq - myfreq(1);

% absolute value of the fft
ffdata = fft(d, nfft);
mypower = abs(ffdata);
myphase = angle(ffdata);

% look at only positive frequencies
mypower = mypower(1:round(end/2));
myphase = myphase(1:round(end/2));
myfreq = myfreq(1:round(end/2));

% restrict frequency range
pts = find(myfreq < flim(2) & myfreq > flim(1));
myfreq = myfreq(pts);
myphase = myphase(pts);
mypower = mypower(pts);
normpower = mypower ./ sum(mypower);

% get max power
[mval, mind] = max(normpower);

% compute power ratio from region proportional to cycle period
maxfreq = myfreq(mind);
maxphase = myphase(mind);

% frequency range that is 10% of cycle period.
% 1 / (1 / f * 1 +/- p%) = f / p%
p = 10; % percent
T = 1/maxfreq;

freqrange = [1/(T+T*(p/100)) 1/(T-T*(p/100))];
% freqrange = [maxfreq / (1+p/100) maxfreq / (1-p/100)];

[~, minind] = min(abs(myfreq - freqrange(1))); % get index
freqinds = [minind];
[~, minind] = min(abs(myfreq - freqrange(2))); % get index
freqinds = [freqinds minind];
power_ratio = sum(normpower(freqinds(1):freqinds(2)));

ff.ffdata = ffdata;  % raw data
ff.power = mypower;  % fft power
ff.phase = myphase;  % fft phase
ff.freq = myfreq;  % range of frequency values
ff.normpower = normpower;  % normalized power in restricted range [a b]
ff.pr.val = power_ratio;  % power ratio under max peak
ff.pr.f = freqrange;  % frequency range of power ratio calc
ff.maxfreq = maxfreq;  % maximum freq. in restricted range [a b]
ff.mfind = mind;  % index of maxfreq in restricted range [a b]
ff.maxphase = maxphase;  % max phase of maxfreq

