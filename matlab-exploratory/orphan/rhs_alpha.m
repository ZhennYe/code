function r = rhs_alpha(alpha)

global nu sigma k L q h g

r = nu - sigma*phi(alpha,g);