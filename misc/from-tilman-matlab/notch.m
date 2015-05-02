% Notch filter
% Adapted from code from Mark Kramer
% params:
%   vm: signal to filter
%   fs: sampling frequency (Hz)
%   varargin: notch frequency (Hz), 60 Hz by default
function vfilt = notch(vm, fs, varargin)
    if (nargin == 3)
        nfreq = varargin{1};
    else
        nfreq = 60;
    end
    Wn1 = [(nfreq-1) * 2 / fs, (nfreq+1) *2 / fs];
	[B1,A1] = butter(2, Wn1, 'stop');
    vfilt = filtfilt(B1, A1, vm);