%% prepares for running scalogram scripts
% simple_cwt.m, my_morlet.m, plot_cwt.m

Fs = 5000;
total_time = 5;
t = (1/Fs):(1/Fs):(total_time/3);
f = [100 500 1000];
x = [cos(f(1)*2*pi*t) cos(f(2)*2*pi*t) cos(f(3)*2*pi*t)];
t = (1/Fs):(1/Fs):total_time;

% add short transients
trans_time = 0:(1/Fs):0.05;
trans_f = 1000;
for secs = 0.5:0.5:4;
    trans = cos(trans_f*2*pi*trans_time);
    x((secs*Fs):(secs*Fs+length(trans)-1)) = trans;
end

%% in prompt:
% scales = 1:200;
% coefs = simple_cwt(t, x, @my_morlet, 10, scales, [10]);
% plot_cwt(t, coefs, scales);