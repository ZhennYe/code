

global nu sigma k kt L q h g %phi

nu = 0.1;
%nu = 0.04;
sigma = 1.2;
k = 0.4;
kt = 0.4;
L = 10^6;
q = 100;
h = 10;
%phi = 1.618;

for i=1:250
g = 0.1*i;
gamma(i) = g;
null_alpha(i) = fzero('rhs_alpha',70);
null_gamma(i) = fzero('rhs_gamma',80);
end

figure(2)
hold on;
plot(gamma,null_alpha,'b');
plot(gamma,null_gamma,'r');
axis([0 25 40 100]);
xlabel('\gamma');
ylabel('\alpha');

