function dz = fallingBody2(~,z)

% Calculate gravity on Venus based on altitude
GM = 3.2494e14;
R = 6.0518e6;
g = GM / ((R+z(1))^2);

% Calculate atmosphere density on Venus based on altitude
r0 = 65;
c1 = -1.2324e-9;
c2 = 1.0109e-5;
rho = r0 * exp(c1*z(1)^2 - c2*z(1));

% Calculate drag coefficient based on atmosphere density
D = rho*0.0033*1.5;

% Calulate the derivatives of z
dz = zeros(size(z));

dz(1) = z(2);
dz(2) = D*z(2)*z(2) - g;
