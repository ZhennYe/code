%% Comparing Relative Errors Between Euler and ODE45 methods
% Comparison of the Euler and ODE45 methods of solving the ODE given by the
% function fallingBody1. The analytic solution is used to calculate the
% exact error with each method.


%% Initialize Initial Conditions and Parameters

v0 = -5700; % initial velocity [m/s]
D  = 3.3e-5; % drag coefficient
g  = 3.72;   % gravity [m/s^2]

% Constants used in calculating the analytic soln
a = -sqrt(g/D);
b = sqrt(g*D);


%% Calculate Relative Errors

% Euler Method
[te,ve] = euler(@fallingBody1,[0 200],v0,0.1);
vth = a*(v0+a*tanh(b*te)) ./ (a+v0*tanh(b*te));
err_e = 100*abs((vth-ve)./vth);

% ODE23 Solver
[to,vo] = ode45(@fallingBody1,[0,200],v0);
vth = a*(v0+a*tanh(b*to))./(a+v0*tanh(b*to));
err_o = 100*abs((vth-vo)./vth);


%% Visualize Results

plot(te,err_e,to,err_o,'LineWidth',2)
plot(te,err_e,'+-','LineWidth',2)
hold all
plot(to,err_o,'s-','LineWidth',2)
title('Comparing Relative Error Between Euler and ODE45 methods')
legend('Euler','ode45')
xlabel('t (s)')
ylabel('Relative Error (%)')
