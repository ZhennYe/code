%% Testing Model Against Analytic Solution
% Using the analytic solution for forward model to test our
% model of ray attentuation and our solution using backslash. 

close all

%% Creating the Analytic Solution
% bAna: attentuated signal values as a function of length along
% detector line (t) calculated from analytic solution
%
% bForward: attentuated signal values as a function of length along
% detector line (t) calculated from forward model

[t,bAna,bForward] = analyticSolution(30,100,0,pi-(pi/100),1);

%%  Solving for Attenuation (Densities) From Signal Values

% Calculating A using a ray tracing algorithm
A = rayTrace(30,100,0,pi-(pi/100));

% Solving for matrix of attentuation (density) values
xmodel_ana = A\bAna;
xmodel_forward = A\bForward;


%% Comparing the solutions
set(gcf,'Color','w')
subplot(2,1,1); 
clims = [0 1];
imagesc(reshape(xmodel_forward,30,30),clims)
title('Forward Model Simulated Densities')
colormap('bone')
axis('equal','tight','off')

subplot(2,1,2); 
imagesc(reshape(xmodel_ana,30,30),clims)
title('Simulated Densities from Analytic Solution')
colormap('bone')
axis('equal','tight','off')

figure
imagesc(reshape(abs(xmodel_forward-xmodel_ana),30,30),clims)
title('Absolute Difference Between the Forward Model and the Analytic Solution')
axis('equal','tight','off')
colorbar
