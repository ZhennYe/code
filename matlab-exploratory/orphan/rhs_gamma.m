function r = rhs_gamma(alpha)

global nu sigma k L q h g

lambda = q*sigma/h;

r = lambda*phi(alpha,g) - k*g;