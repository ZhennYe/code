% FITUSLXCONSUME.M
%  Fits a model to US Electricity consumption data

%% Load and plot data
load USElectricityInterp

figure(1)
hp1 = plot(t,R,'.-');
xlabel('Date','FontSize',16), ylabel({'Retail consumption';'(10^9 kWh/day)'},'FontSize',16)
set(gca,'FontSize',14)
axis tight
set(gcf,'Position',[373   225   933   483])

%% Build Vandermonde matrix
%  Shift t
x = (t-2000);
n = length(R);
%  Make matrix
M = [ones(n,1),x,x.^2,x.^3,cos(2*pi*x),cos(4*pi*x)];

%% Compute least-squares solution
a = M\R     %#ok<*NOPTS>

%% Evaluate model (using least-squares parameters)
Rmodel = M*a;

%  Plot model on top of data
figure
plot(t,R,'k.-')
line(t,Rmodel,'color','r','linewidth',2)
xlabel('Date','FontSize',16)
ylabel({'Retail consumption';'(10^9 kWh/day)'},'FontSize',16)
legend('Data','Model','Location','NW')
set(gca,'FontSize',14)
axis tight
set(gcf,'Position',[373   225   933   483])

%% Compute best fit statistics
resid = Rmodel-R;       % residuals
sserr = resid'*resid;   % sum of square error
svar = R-mean(R);       % variation from mean (of the data)
sstot = svar'*svar;     % sum of square variations

% R-squared and adjusted R-squared statistics
% These show goodness of fit: 0 = useless, 1 = perfect
% Adjusted R-squared is "fairer" because it accounts for the number of
% variables in the model -- adding another variable will increase the
% R-squared value, but the adjusted R-squared will increase only if the new
% variable improves the model more than can be expected by chance.
R2 = 1 - sserr/sstot 
adjR2 = 1 - (1-R2)*(n-1)/(n-size(R,2)-1)

%% Verify that backslash returns least-square solution
%  Calculate least-square coefficients using explicit formula
%  E = R - Fc
%  => total square error = E'E = (R - Fc)'(R - Fc)
%                               = R'R - c'F'R - R'Fc + c'F'Fc
%  Find least-square solution by differentiating w.r.t. c and setting equal
%  to 0:
%  d(TSE)/dc = -2F'R + 2F'Fc = 0
%  => (F'F)c = F'R
%  Note that this is a square system, so it has a unique solution:
a1 = (M'*M)\(M'*R);
%  Calculate difference between two solutions
norm(a-a1)
