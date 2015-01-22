%% Solve the Spacecraft Landing Problem for the fallingBody3 Model
%  Use the function fallingBody3 as the system to solve.
%

%% Initialize Initial Conditions
v0 = 5700;
theta = -70*pi/180;

z1_0 = 0;
z2_0 = v0*cos(theta);
z3_0 = 250e3;
z4_0 = v0*sin(theta);

z0 = [z1_0;z2_0;z3_0;z4_0];

%% Solve fallingBody3 Using ode45
[t,z] = ode45(@fallingBody3,[0 300],z0);

%% Visualize Results

% Plot position in X and Y directions
figure(1)
subplot(2,1,1)
plot(t,z(:,1),'LineWidth',2)
title('Position')
xlabel('time (s)'), ylabel('x(t) (m)')
subplot(2,1,2)
plot(t,z(:,3),'LineWidth',2)
xlabel('time (s)'), ylabel('y(t) (m)')

% Plot velocity in X and Y directions
figure(2)
subplot(2,1,1)
plot(t,z(:,2),'LineWidth',2)
title('Velocity')
xlabel('time (s)'), ylabel('x''(t) (m/s)')
subplot(2,1,2)
plot(t,-z(:,4),'LineWidth',2)
xlabel('time (s)'), ylabel('| y''(t) |  (m/s)')

% Plot trajectory on the X-Y plane
figure(3)
plot(z(:,1),z(:,3),'LineWidth',2)
title('Postion on the X-Y Plane')
xlabel('x'), ylabel('y')
axis equal
% axis([0,2.5e5,0,2.5e5])
