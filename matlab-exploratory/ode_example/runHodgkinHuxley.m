% run hodgkinHuxley and plot

[t y] = ode45(@hodgkinHuxley,[0 40],[0 .5 .5 .5]);

% Plots time series for voltage and the gating variables

subplot(2,2,1); plot(t,y(:,1)-70); xlabel('Time'); ylabel('Voltage (mV)'); title('Voltage Time Series');
subplot(2,2,2); plot(t,y(:,2)); xlabel('Time'); ylabel('Probability of Opening'); title('M Gating Time Series');
subplot(2,2,3); plot(t,y(:,3)); xlabel('Time'); ylabel('Probability of Opening'); title('H Gating Time Series');
subplot(2,2,4); plot(t,y(:,4)); xlabel('Time'); ylabel('Probability of Opening'); title('N Gating Time Series');