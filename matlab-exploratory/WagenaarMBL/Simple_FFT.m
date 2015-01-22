%% plotting power spectrum (stationary signals)

%need: signal (sig), sampling_interval, 
sampling_interval = 0.00005;

subplot(4, 1,1), plot(sig);

% run FFT of signal
L=length(sig);
Y = fft(sig)/L;

% calculate nyquist limit
NyLimit = (1 / sampling_interval)/2;

% plot fft
hold on;
F = linspace(0,1,L/2)*NyLimit;
subplot(4,1,2), plot(F, abs(Y(1:L/2)));

% find power spectrum of signal
power=(Y(1:L/2)).*conj(Y(1:L/2));
subplot(4,1,3), plot(F, (Y(1:L/2)).*conj(Y(1:L/2)));

%  phase analysis & coherence - fourier components can have various phases
%       relative to one another
phi = atan(imag(Y)./real(Y));   % inverse tangent function
subplot(4,1,4), plot(F, phi(1:L/2));
