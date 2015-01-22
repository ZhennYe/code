function polyfitting(n)

x = linspace(0,1,n);
sz = size(x);
y = x + 0.05*randn(sz);

c = polyfit(x,y,n-1);
xf = linspace(0,1);
plot(x,y,'o',xf,polyval(c,xf),'LineWidth',2)