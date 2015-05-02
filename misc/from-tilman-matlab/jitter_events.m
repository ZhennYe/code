% Take an event trace and and some random jitter to each event.
% This can be useful to "smooth" out highly regular trains of events
% which leads to very sharp peaks in the FFT and strong harmonics
% times: event trace
% factor: scale factor of jitter to use. Scaled relative to mean/median
% interval
function d = jitter_events(times, factor)

inter = mean(diff(times));
for i = 1:length(times)
    delta = randn * factor * inter;
    % fprintf('%d: changing %.2f s to %.2f s\n', i, times(i), (times(i) + delta));
    times(i) = times(i) + delta;
end
d = times;