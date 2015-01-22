%% X-ray Tomography Example of an Overdetermined Realistic System
% This X-ray tomography example shows how matrix computations in MATLAB can
% be applied to solve a more realistic system using backslash (\).
% 
% In this example, a more realistic object model is generated using the
% function PHANTOM. The ray attentuation (representing object densities) of
% the model is calculated from the measured intensities of the individual
% ray paths. The model can be expressed by the following equation,
%
% $$ Ax = b $$
%
% Where:
%
% <html>
% &nbsp;&nbsp;<i>A</i> - weights on the cells for all ray paths<br>
% &nbsp;&nbsp;<i>x</i> - density value for each element<br>
% &nbsp;&nbsp;<i>b</i> - observed intensities for each detector
% </html>


%% Part I - The Forward Model: Obtain “Observed” X-Ray Intensity from Attenuation Model

clear
close all
clc

% Load the measured intensities b vectors, 5000 elements long
load measuredIntensitiesPhantomModel

% Creating matrix A of ray paths
% Setting the ray paths for the 50x50 model
% 100 different angles for each ray, 50 emitters
% This will yield a 5000x2500 matrix
m = 50;     % length of side
n = 100;    % number angles for the ray paths
minAngle = 0;
maxAngle = pi-(pi/100);

A = rayTrace(m,n,minAngle,maxAngle);


% Part II - The Inverse Model: Obtain Attenuation (Densities) From Observed X-Ray Intensity

xModel = A\b;
xModel_noise = A\b_noise;


%% Visualize the Attenuation Models
% Compare the results from the ideal model and the more realistic model
% incorporating simulated measurement noise.

% Reshape the models
xModel = reshape(xModel,m,[]);
xModel_noise = reshape(xModel_noise,m,[]);

% Visualize models
figure('Color','w')
subplot(2,1,1)
imagesc(xModel,[0 1])
axis('equal','tight','off')
colormap('bone')
title('Ideal Attenuation Model')

subplot(2,1,2)
imagesc(xModel_noise,[0 1])
axis('equal','tight','off')
colormap('bone')
title({'Realistic Attenuation Model';'(from noisy measurements)'})
