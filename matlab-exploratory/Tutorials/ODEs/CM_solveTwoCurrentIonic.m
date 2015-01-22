%% Comparing Solutions of Different Solvers for a Stiff System
% The system is modeled by twoCurrentIonic.m.  We use several different
% solvers to solve the system and compare the output.
%


%% Initialize Parameters
tspan = [0,400];
x0 = [0.1;0.7];

options = odeset('RelTol',1e-3); % default 1e-3


%% Solve the Two Current Ionic Model Using Various Solvers

% ode45
[t_45,x_45] = ode45(@twoCurrentIonic,tspan,x0,options);
n_45 = num2str(numel(t_45)); % Calculate number of points

% ode23
[t_23,x_23] = ode23(@twoCurrentIonic,tspan,x0,options);
n_23 = num2str(numel(t_23)); % Calculate number of points

% ode23s (stiff solver)
[t_23s,x_23s] = ode23s(@twoCurrentIonic,tspan,x0,options);
n_23s = num2str(numel(t_23s));% Calculate number of points

% ode15s (stiff solver)
[t_15s,x_15s] = ode15s(@twoCurrentIonic,tspan,x0,options);
n_15s = num2str(numel(t_15s));% Calculate number of points


%% Visualize Results

% Overall Results
figure(1)
plot(t_45,x_45(:,1),...
     t_23,x_23(:,1),...
     t_23s,x_23s(:,1),...
     t_15s,x_15s(:,1),...
     'LineWidth',4)

title('Comparing Results of Various Solvers')
xlabel('t (ms)'), ylabel('V')
legend(['ode45 - ' n_45 ' points'],...
       ['ode23 - ' n_23 ' points'],...
       ['ode23s - ' n_23s ' points'],...
       ['ode15s - ' n_15s ' points'])

% Zoomed in
figure(2)
axisLim = [0,50,0.9,0.95];

subplot(2,2,1)
plot(t_45,x_45(:,1),'LineWidth',2)
axis(axisLim)
title('ode45')
xlabel('t (ms)'), ylabel('V')

subplot(2,2,2)
plot(t_23,x_23(:,1),'LineWidth',2)
axis(axisLim)
title('ode23')
xlabel('t (ms)'), ylabel('V')

subplot(2,2,3)
plot(t_23s,x_23s(:,1),'LineWidth',2)
axis(axisLim)
title('ode23s')
xlabel('t (ms)'), ylabel('V')

subplot(2,2,4)
plot(t_15s,x_15s(:,1),'LineWidth',2)
axis(axisLim)
title('ode15s')
xlabel('t (ms)'), ylabel('V')

