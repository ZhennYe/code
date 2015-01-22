function y = consumptionmodel(x)

a = [ 9.1503 ; 0.1880 ; -0.0052 ; -0.0005 ; -0.4645 ; 0.7059 ];
y = [ 1  x  x.^2  x.^3  cos(2*pi*x)  cos(4*pi*x) ] * a - 10;