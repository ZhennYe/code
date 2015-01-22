% create a time domain trace from single events (for example spikes)
% the resulting trace will be all zeros with ones where spikes occured
% et: event times 
% fs: sampling frequency
% len: total desired data length
function [time, y] = make_event_trace(et, fs, len)

dt = 1/fs;
events = round(et * fs);
% make sure events are within trace limits
events((events < 1) | (events > len * fs)) = [];
time = 0:dt:len-dt;
y = zeros(length(time),1);
y(events) = 1;
    