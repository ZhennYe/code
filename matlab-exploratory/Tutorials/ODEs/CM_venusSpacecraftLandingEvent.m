%% Solve the Spacecraft Landing Problem with Event Handling
% Use the function eventLanding.m as an input for the Events property of
% ODESET.  This will allow ODE45 to determine when the spacecraft lands
% (when the vertical height equals zero) and return the values for the
% velocity and location as well as time for this event.
%

%% Initialize Initial Conditions
v0 = 5700;
theta = -70*pi/180;

z1_0 = 0;
z2_0 = v0*cos(theta);
z3_0 = 250e3;
z4_0 = v0*sin(theta);

z0 = [z1_0;z2_0;z3_0;z4_0];


%% Setup Event option and Solve System
options = odeset('Events',@eventLanding);
[t,z,te,ze] = ode45(@fallingBody3,[0 inf],z0,options);

disp(['The time of landing is ' num2str(te) ' s'])
disp('At the final time,')
disp(['x     = ' num2str(ze(1)) ' m'])
disp(['x-dot = ' num2str(ze(2)) ' m/s'])
disp(['y     = ' num2str(ze(3)) ' m'])
disp(['y-dot = ' num2str(ze(4)) ' m/s'])


%% Visualize Results

% Displacement
figure(1)
subplot(211)
plot(t,z(:,1),t,z(:,3),'LineWidth',3)
title('Displacement Over Time of the Spacecraft')
xlabel('Time (s)'), ylabel('Distance (m)')
legend('X-direction','Y-direction', 'Location','NE')
axis tight

% Trajectory
subplot(212)
plot(z(:,1),z(:,3),'LineWidth',3)
title('Trajectory of the Spacecraft in the XY-Plane')
xlabel('x'), ylabel('y')
axis tight
