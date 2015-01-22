 function [tscaled,pAna,pForward] = analyticSolution(nn,nangles,minangle,maxangle,vis)
 
%% Comparison with analytic solution
% Reference for analytic solution:
% http://www.slaney.org/pct/pct-toc.html  (Principals of Computerized
% Tomography, Originally published by IEEE Press.  (C) 1988 Institute for
% Electrical and Electronic Engineers. Electronic copy (C) 1999 A. C. Kak
% and Malcolm Slaney)
% 
% INPUTS:
% =======
%  nn : number of X-rays
%  nangles : number of distinct angles at which to take measurements
%  minangle : smallest angle for data collection
%  maxangle : largest angle for data collection
%  vis : boolean {0,1} describing if the output is plotted (1) or not (0)
%  
% OUTPUTS:
% ========
%  tscaled
%  pAna : the measured intensities of the X-rays after they have penetrated
%         the body.  These intensities are calculated using the analytic,
%         or exact, model.
%  pForward : the measured intesities of the X-rays using our estimated
%             model that we naively calculated.  We simply account for the
%             X-ray passing through a block of the body but we do not
%             account for which fraction of the X-ray passes through which
%             blocks.

angles = linspace(minangle,maxangle,nangles);

pAna = zeros(nangles*nn,1);
pForward = zeros(nangles*nn,1);

% Ellipse Parameters
rho = 1.04;
AA = 0.4;
BB = 0.3;

x1 = 0.2;
y1 = 0.2;

for jj = 1:length(angles)
    
    theta = angles(jj);

    % Analytic Solution
    
    P = zeros(1,nn);
    t = linspace(-1,1,nn);
    s = sqrt(x1^2 + y1^2);
    gamma = atan(y1/x1);
    
    ts = t - s*cos(gamma+theta); 
    
    if isnan(ts(1))
        ts = t;
    end
    
    a = sqrt(AA^2*cos(theta)^2 + BB^2*sin(theta)^2);
    
    idx = (abs(ts)<= a);
    
    P(idx) = (2*rho*AA*BB)/a^2*sqrt(a^2 - ts(idx).^2);
    
    % Forward Model
    
    % Setting up the grid
    ngrid = nn + 1;
    
    x = linspace(0,1,ngrid);
    y = linspace(0,1,ngrid);
    
    % Scaling, Analytic Solution -1 to 1.
    % RayTrace assumes 0 to 1.
    
    x1A = (x1 + 1)/2 ;
    y1A = (y1 + 1)/2 ;
    
    AAA = AA/2;
    BBB = BB/2;
    
    [xgrid ygrid] = meshgrid(x,y);
    xMid = xgrid + 0.5*(x(2)-x(1));
    yMid = ygrid + 0.5*(y(2)-y(1));
    
    xMid = xMid(1:end-1,1:end-1);
    yMid = yMid(1:end-1,1:end-1);
    
    idx = ((xMid-x1A).^2./AAA.^2 + (yMid-y1A).^2./BBB.^2 <=1);
    XX = zeros(nn,nn);
    XX(idx) = rho;
    
    % Solving for Forward Model
    
    A = rayTrace(nn,1,1,theta);

    b = A*XX(:);
    
    counter = (jj-1)*(nn)+1;
    
    pAna(counter:counter+nn-1,1) = P'./2; 
    pForward(counter:counter+nn-1,1) = b; 
    
    tscaled = (t+1)/2;
    
    if vis == 1
        plot(xMid(1,:),b,'o',tscaled,P./2)
        pause(0.5)
    end
    
end
 

