function indeces = spike_detect(data, N, varargin)
% Mike Economo modified by Tilman
% August 2006
% This is a fairly robust AP detector
% Inputs: 
%   data: voltage waveform
%   N: refractory period between spikes (number of samples)
%   varargin: set threshold

% different options for threshold
% 1. mean voltage + value (mV)
% 2. fixed value (mV)
% 3. mean voltage + 3 standard deviations (mV)

if (nargin == 3)
    thresh = varargin{1};
else
%     thresh = mean(data) + 10;  
    % thresh = -30;  
    thresh = mean(data) + 4 * std(data);  
end

% derivative of the signal at which to mark a spike
dvdt = 1;
% window (number of samples) over which to compute derivate
win = 1;  

indeces = [];
spike = 0;
for i = (win+1):length(data)
    % first method is pure threshold, second is derivative
    if ((data(i-1) <= thresh && data(i) > thresh) && spike < 1)
%     if (((data(i) - data(i-win)) > dvdt*win) && spike < 1)
        indeces(end+1) = i;
        spike = N;
    end;
    spike = spike-1;
end

