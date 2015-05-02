% time: time values
% data: lvn data
% temp: recording temp
% r: row in the figure to plot
% fn: data file name
% nrow: how many total rows we are plotting
% dpath: data directory path
function plot_power_ratio(time, data, fs, temp, r, fn, nrow, dpath)

% figure out the row indecies
ncol = 4;
rawspan = [1 2];
plotspan = [ncol * (r-1) + rawspan(1) ncol * (r-1) + rawspan(2)];
fftcol = ncol * r - 1;
xcorrcol = ncol * r;
speccol = ncol * r;
histcol = ncol * r;

% optionally downsample data (low pass filter), go down to 5 kHz
% this makes plotting a lot faster and the data easier to work with
ds = 0;
if (ds == 1)
    refs = round(fs / 5e3);
    tt = downsample(time, refs);
    dd = resample(data, 1, refs);
    data = dd; time = tt;  % use downsampled data
    fs = round(fs / refs);
end

axis_options = { 'Spacing', 0.02, 'SpacingVert', 0.00, ...
                 'Padding', 0.01, ... 
                 'Margin', 0.05, 'MarginRight', 0.03, 'MarginBottom', 0.06};

% smooth the data
% win = 100 / 1000 * fs; % ms 
% tau = 40;
% data = mne_smooth(data, win, tau);
% data = data + randn(length(data),1) * max(data)/10;


             
lims = [0 round(length(data)/fs)]; % sec
lims = [0 60];
lims = [lims(1)*fs+1 lims(2)*fs];
ax(1) = subaxis(nrow, ncol, [plotspan(1) plotspan(2)], axis_options{:});
tl = time(lims(1):lims(2)); dl = data(lims(1):lims(2));
% plot(tl, dl, 'color', [0.5 0.5 0.5], 'LineWidth', 2); hold on;
event_times = time(data == 1);

rasterplot(event_times * 20e3, 1, 60*20e3, ax(1), 20e3);
set(gca, 'LineWidth', 1);

ylabel('lvn Voltage (V)');% grid on;
legend({sprintf('%s | Temp: %.1f %cC', fn, temp, char(176))}, 'Interpreter', 'none');
% title(sprintf('Temp: %.1f %cC', temp, char(176)));
prettyPlot(0.005); 
ylim([-0.1 1.1]);

if (r == nrow); xlabel('Time (s)'); 
else set(gca, 'XTickLabel', []); end
if (r == 1); title(sprintf('%s | %s', date, dpath), 'Interpreter', 'none'); end;

% optional preprocessing
% inds = spike_detect(dl, 100);
% plot(tl(inds), dl(inds), 'r.');
% [st, sks] = make_event_trace(time(inds), fs, length(data)/fs);
% fprintf('std: %.2f | max: %.2f mean event: %.2f\n', std(data), max(data), mean(data(inds)));
data = abs(data);

% my own custom FFT wrapper function
flims = [0.1 4];  % frequency limits
dfft = 10;  % segment nfft by this factor
ff = getfft(data, fs, flims, dfft);

% plot the FFT
% ax(2) = subaxis(nrow, ncol, fftcol, axis_options{:});
% plot(ff.freq, ff.normpower, 'r'); hold on;
% mark the max frequency point
% maxnp = max(ff.normpower(ff.mfind));
% plot(ff.maxfreq, maxnp, 'k.');

% add power ratio and bar
% plot(ff.pr.f, [maxnp/2 maxnp/2], 'k', 'LineWidth', 5);
% text(ff.pr.f(2), maxnp/2, sprintf(' PR = %.2f @ %.2f Hz', ...
%     ff.pr.val, ff.maxfreq));
% legend(sprintf('npts: %d', length(ff.normpower)));
% prettyPlot(); grid on; ylabel('Norm. Power');
% if (r == nrow); xlabel('Frequency (Hz)'); 
% else set(gca, 'XTickLabel', []); end



% over lay phase adjusted cos wave at most prominent frequency
% yy = get(ax(1), 'ylim');
% wave = cos(ff.maxfreq * time * 2 * pi + ff.maxphase);
% wave = (wave + 1) ./ 2;
% wave = wave * (yy(2) - yy(1)) - yy(1);
% plot(ax(1), time(lims(1):lims(2)), wave(lims(1):lims(2)), 'g', 'LineWidth', 2);
% title(ax(1), sprintf('phase: %.2f \\pi radians (%.2f)', ff.maxphase / pi, ff.maxphase));

% ISI Histogram
% only works for event data (ie. when data is a series of 0 and 1)
ax(3) = subaxis(nrow, ncol, fftcol, axis_options{:});
event_times = time(data == 1);
nbins = 70;
bins = linspace(0, 2, nbins);
interv = diff(event_times);
N = histc(interv, bins);
bar(bins, N, 'histc');
legend(sprintf('Mean: %.2f s | CV: %.2f', ...
    mean(interv), std(interv) / mean(interv)));
ylabel('Count'); prettyPlot();
prettyHist('r','w'); xlim([0 1]);
vline(median(interv),'k--',sprintf('median: %.2f',median(interv)));
ylim([-2 45]); %makeLogHist();

if (r == nrow); 
    xlabel('Burst Intervals (s)');
else
    set(gca, 'XTickLabel', []);
end

% return;

% spectrogram
ax(3) = subaxis(nrow, ncol, speccol, axis_options{:});
if (1) % high-res spectrogram
    win = length(data)/10;
    noverlap = 0.9 * win;
    freqpoints = length(data) / 2;
else % lower-res spectrogram (fast) to compute
    win = length(data)/10;
    noverlap = 0.85 * win;
    freqpoints = length(data) / 10;
end


tstart = tic;
% [S,F,T,P] = spectrogram(data, win, noverlap, ff.freq, fs);
[S,F,T,P] = spectrogram(data, win, noverlap, freqpoints, fs);
telapsed = toc(tstart);
% restrict the range (only if using dfft argument to spectrogram)
freq = F;
P = P(freq >= flims(1) & freq < flims(2), :);
F = F(freq >= flims(1) & freq < flims(2));
fprintf('Time: %.2f s | tbins: %d | fbins %d\n', telapsed, length(T), length(F));

% do some custom plot pretty-fying
if (r == 1)
P = P(freq >= flims(1) & freq < 2, :);
F = F(freq >= flims(1) & freq < 2);
end    
surf(T, F, P, 'edgecolor','none'); 

view(0, 90); ylabel('Frequency (Hz)');
set(gcf,'Renderer','Zbuffer'); % need this to render nicely
prettyPlot(); grid off;
ylim([0 4]);
if (r == nrow); 
    xlabel('Time (s)'); 
else
    set(gca, 'XTickLabel', []);
end


% auto correlation 
% ax(3) = subaxis(nrow, ncol, xcorrcol, axis_options{:});
% smooth the data
% win = 50 / 1000 * fs; % ms 
% tau = 10;
% data = mne_smooth(data, win, tau);
% plot(ax(1), time(lims(1):lims(2)), data(lims(1):lims(2)), 'b', 'LineWidth', 1);
% lag = 2; % sec
% xc = xcorr(data, data, lag * fs, 'unbiased');
% plot(-lag:1/fs:lag, xc, 'Color', [0.5 0.5 0.5]);
% xlim([0.1 lag]); 
% prettyPlot();
% ylabel('Correlation');
% 
% if (r == nrow); 
%     xlabel('Time Lag (s)'); 
% else
%     set(gca, 'XTickLabel', []);
% end

