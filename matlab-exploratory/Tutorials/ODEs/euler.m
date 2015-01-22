function [t,x] = euler(f,tspan,x0,h)
%EULER  Solve first order ODEs with fixed-step Euler's method
%
%  [T,X] = EULER(F,TSPAN,X0,H) with TSPAN = [T0 TFINAL] integrates the
%  system of differential equations dy/dt = f(t,y) from t = T0 to 
%  t = TFINAL, starting with the initial condition is y(T0) = Y0, using a
%  fixed stepsize of H.
%
%  If TFINAL - T0 is not divisible by an integer multiple of the stepsize
%  H, H is resized appropriately.

% Copyright 1984-2011 The MathWorks, Inc. 

%  Determine number of steps needed, rounded to nearest integer
N = round((tspan(2)-tspan(1))/h);
%  Recalculate stepsize
% (in case requested stepsize didn't evenly divide time interval)
h = (tspan(2)-tspan(1))/N;
%  Generate t values
t = linspace(tspan(1),tspan(2),N+1)';
%  Set aside space for solution
x = zeros(N+1,length(x0));
%  Add initial condition to solution
x(1,:) = x0';

%  Do N steps of Euler's method
for k=1:N
    x(k+1,:) = (x(k,:) + h*f(t(k),x(k,:)'))';   % Step forward
end
