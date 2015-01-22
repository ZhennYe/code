function dydt = simple_LP(t,y) %t is a vector, y is an initial condition vector
% this function contains only delayed rectifier current

% y(1) = voltage
% y(2) = n gating variable

% constants for Id
gd=0.35e-6;
Ek=-80e-3;
cn=180;
Vn=-25e-3;
Vkn=10e-3;
sn=-7e-3;
skn=-22e-3;

ninf = 1 / (1 + exp(((y(1))-Vn)/sn));

kn = cn / (1 + exp((y(1) - Vkn) / skn));

dydt=zeros(2,1);
nn = ( ninf - y(2)) * kn * dt;
Id = gd * (nn^4) * (y(1) - Ek);
dydt(2) = nn;
dydt(1) = Id;
