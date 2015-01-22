%% nullclines and phase planes

% initialize for plotting (fake data)
x=-10:10;
y=x;
[X,Y]=meshgrid(x,y);
f=X+Y;
g=4*X+Y;

hold on;
% plot DE field
quiver(X,Y,f,g);


