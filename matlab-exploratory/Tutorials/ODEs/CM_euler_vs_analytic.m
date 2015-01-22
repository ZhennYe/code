%% Implementing Euler's Method to Solve ODEs
% CM_EULER_VS_ANALYTIC
% Comparing Euler's Method versus the analytic solution.  Euler's Method is
% an iterative method where the previous velocity value is used with the
% time step in order to determine the subsequent time step.  The analytic
% solution was derived from the original differential equation and is given
% below:
% 
% $$v = \sqrt{\frac{g}{D}}\cdot\frac{1+\frac{v0-\sqrt{\frac{g}{D}}}
% {v0+\sqrt{\frac{g}{D}}}\cdot e^{2t\sqrt{gD}}}
% {1-\frac{v0-\sqrt{\frac{g}{D}}}
% {v0+\sqrt{\frac{g}{D}}}\cdot e^{2t\sqrt{gD}}}$$
%
% Copyright 1984-2011 The MathWorks, Inc. 


%% Initialize Initial Conditions, Parameters, and Variables

% Initial Conditions
t0 = 0;         % initial time [s]
v0 = -5700;     % initial velocity [m/s]

% Parameters
D  = 3.3e-5;    % drag coefficient
g  = 3.72;      % gravity [m/s^2]

% Create Time Vector
h  = 1;             % time step [s]
tf = 200;           % final time [s]
t  = (t0:h:tf)';    % time vector
N  = length(t);     % number of steps

% Initialize Velocity Vector
v = zeros(size(t));


%% Apply Euler's Method to Calculate the Velocity at Each Time Step
v(1) = v0;  % Set the initial velocity value

for k = 1:N-1
    % Calculate the derivative of velocity from the ODE
    f = rateEqn(t(k),v(k));
    
    % Calcualte the approximate velocity at the next time step
    v(k+1) = v(k) + h*f;
end


%% Calculate Analytic Solution
% Use the formula to calculate the exact solution to the ODE.
% $$v = \sqrt{\frac{g}{D}}\cdot\frac{1+\frac{v0-\sqrt{\frac{g}{D}}}
% {v0+\sqrt{\frac{g}{D}}}\cdot e^{2t\sqrt{gD}}}
% {1-\frac{v0-\sqrt{\frac{g}{D}}}
% {v0+\sqrt{\frac{g}{D}}}\cdot e^{2t\sqrt{gD}}}$$

v_analytic = sqrt(g/D) * ...
    (1 + (v0-sqrt(g/D))/(v0+sqrt(g/D)) * exp(2*t*sqrt(g*D))) ./ ...
    (1 - (v0-sqrt(g/D))/(v0+sqrt(g/D)) * exp(2*t*sqrt(g*D)));


%% Visualize and Compare Results

plot(t,v,'b.-',t,v_analytic,'r','LineWidth',2,'MarkerSize',20)

title('Comparing Euler''s Method against the Analytic Solution')
legend('Euler''s Method','Analytic Solution','Location','SE')
xlabel('Time (s)')
ylabel('Velocity, v (m/s)')

