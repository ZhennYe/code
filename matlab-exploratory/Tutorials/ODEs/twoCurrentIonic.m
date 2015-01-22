function dx = twoCurrentIonic(~,x)
% Two Current Ionic Model for Cardiac Action Potential 

% Initialize parameters
dx = zeros(size(x));
v = x(1);   % Voltage
h = x(2);   % Conductance

% Calculate the derivatives of x
dx(1) = h .* v .* v .* (1-v)/0.1 - v/2.4;

if (v <= 0.13)
    dx(2) = (1-h)/130;
else
    dx(2) = -h/150;
end
