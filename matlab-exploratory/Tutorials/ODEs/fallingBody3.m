function dz = fallingBody3(~,z)

% Gravity on Venus based on altitude
GM = 3.2494e14;
R = 6.0518e6;
g = GM / ((R+z(3))^2);

% Atmosphere density on Venus based on altitude
r0 = 65;
c1 = -1.2324e-9;
c2 = 1.0109e-5;
rho = r0 * exp(c1*z(3)^2 - c2*z(3));

% Drag coefficient based on atmosphere density
D = rho*0.0033*1.5;

% Net velocity of spacecraft
v = sqrt(z(2).^2 + z(4).^2);

% Calulate the derivatives of z
dz = zeros(size(z));

dz(1) = z(2);
dz(2) = -D*z(2)*v;
dz(3) = z(4);
dz(4) = -D*z(4)*v - g;
