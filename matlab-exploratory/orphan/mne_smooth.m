% smooth by convolving with a gaussian
% written by Mike Economo
% params:
%   x: waveform to smooth
%   halflen: window to smooth over
%   tau: time constant of the gaussian
function y = mne_smooth(x, halflen, tau)
    pts = (-halflen:halflen)';
    win = exp(-(pts ./ tau) .^ 2);
    y = conv(win, x);
    norm = conv(win, ones(size(x)));
    y = y ./ norm;
    y = y(halflen + 1:halflen + length(x));