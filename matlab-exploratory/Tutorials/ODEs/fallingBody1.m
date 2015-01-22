function dv = fallingBody1(t,v)

% Initialize parameters for Mars
D = 3.3e-5; % drag coefficient
g = 3.72;   % gravity [m/s^2]

% Calulate the first derivative of v
dv = D*v.^2 - g;
