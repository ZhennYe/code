function A = rayTrace(nn,nangs,varargin)
% Calculates ray paths for a larger realistic tomography example
%
% nn - number of elements along each side of the grid, assumed square
% nangs - number of angles calculated, nn rays calculated at each angle
% minangle - minimum angle value
% maxangle - maximum angle value

if nargin > 4
    warning(['Too many input arguments.',char(10),...
             'RAYTRACE takes 4 inputs, but was given ',num2str(nargin)])
end

minangle = 0;
maxangle = 3*pi/4;
if nargin > 2
    minangle = varargin{1};
    if nargin > 3
        maxangle = varargin{2};
    end
end


if nn < 5 && nangs < 5
    vis = 1;
else
    vis = 0;
end

%% Setting up the grid
ngrid = nn + 1;  % number of points in grid
x = linspace(0,1,ngrid);
y = linspace(0,1,ngrid);
hS = []; % setting figure handle to empty to start

[xgrid ygrid] = meshgrid(x,y);
xMid = xgrid + 0.5*(x(2)-x(1));  % calculating midpoint of each element
yMid = ygrid + 0.5*(y(2)-y(1));  % calculating midpoint of each element

xMid = xMid(1:end-1,1:end-1);  %removing edge point
yMid = yMid(1:end-1,1:end-1);  %removing edge point

angles = linspace(minangle,maxangle,nangs); %angles used
A = zeros(length(angles)*length(xMid), numel(xMid)); %preallocating ray matrix

%% Tracing ray paths for every angle
% Looping over angles
for aa = 1:length(angles)
    
    angle = angles(aa);
    [xvals,yvals,bvals,bsides,lpt,m] = rayInfo(angle,x,y);
    
    if vis == 1
        % plotting ray paths
        % returns handle to scatter plot
        hS = plotRayPaths(xMid,yMid,xvals,yvals);
    end
    
    % Calculating A matrix representing raypaths
    % If vis ==1, then points corresponding to those elements set to 1
    % in A will be visualized
    A = rayPathsMat(nn,aa,angle,m,lpt,bsides,bvals,xMid,yMid,hS,vis,A);
    
end
end

function hS = plotRayPaths(xMid,yMid,xvals,yvals)
hS = scatter(xMid(:),yMid(:),24,'filled');

for ii = 1:length(xvals)
    a(ii) = line(xvals(:,ii),yvals(:,ii)); %#ok<AGROW,NASGU>
end

xlim([0 1]);
ylim([0 1]);
pause(0.5)  % pausing just for the animation
end

function hS = plotPoints(hS,xMid,yMid,idx)
XData = xMid(:);
YData = yMid(:);
XData(~idx(:)) = [];
YData(~idx(:)) = [];
set(hS,'XData',XData,'YData',YData);
xlim([0 1]);
ylim([0 1]);
pause(0.5) % pausing just for the animation
end

function [xvals,yvals,bvals,bsides,lpt,m] = rayInfo(angle,x,y)
lx = max(x);
ly = max(y);

% Points along the x-ray emitter line
lpt = x + 0.5*(x(2)-x(1));
lpt = lpt(1:end-1);

% Calculating b, defined as y value when x is zero First, calculate b for
% ray through center of emitter line (borg) Then, calculate bvalues for all
% rays (bvals) and finding end and start points for rays

if mod(angle,pi/2) <= 0.01
    % accounting for horizontal or vertical lines, allowing some wiggle room
    m  = 0;
    borg = ly/2;
    bvals = borg - (lpt - (lx/2)); 
    bsides = borg - (x-(lx/2));
else
    borg = (ly/2) - (lx/2)*(1/tan(angle));
    m = ((ly/2) - borg)/(lx/2);
    bvals = borg - ((lpt - (lx/2))/sin(angle));
    bsides = borg - ((x - (lx/2))/sin(angle));
end

if mod(angle,pi) <= 0.01  % accounting for vertical lines
    yvals = [zeros(1,length(bvals)); 2*ones(1,length(bvals))];
    xvals = [lpt; lpt];
    bvals = NaN * length(lx);
else
    yvals = [bvals; bvals + 2*m];
    xvals = [zeros(1,length(bvals)); 2*ones(1,length(bvals))];
end

end

function A = rayPathsMat(nn,aa,angle,m,lpt,bsides,bvals,xMid,yMid,hS,vis,A)

% if the ray + 0.5 or - 0.5 ray spacing width intersects
% the center of the cell - we put a 1 in the cell, otherwise a 0

if mod(angle,pi) <= 0.01; % accounting for vertical lines
    
    for ib = 1:length(lpt)
        idx = abs(xMid - lpt(ib)) < 1e-4;
        offset = (aa-1)*length(lpt);
        lx = 1;
        scale = lx/nn; % normalize by number of elements
        
        A(offset+ib,:) = scale.*idx(:);
        
        if vis == 1
            hS = plotPoints(hS,xMid,yMid,idx);
        end
        
    end
    
else
    
    for ib = 1:length(bvals)
        
        bsideL = bsides(ib);
        bsideR = bsides(ib+1);
        idx1 = (m*xMid + bsideL - yMid < 0);
        idx2 = (m*xMid + bsideR - yMid > 0);
        idxEq = abs(m*xMid + bsideL - yMid) == 0;
        idxEq2 = abs(m*xMid + bsideR - yMid) == 0;
        idx = ~(idx1+idx2)| idxEq | idxEq2;
        
        offset = (aa-1)*length(bvals);
        
        lx = 1;
        scale = lx/nn;
        
        A(offset+ib,:) = scale.*idx(:);
        
        %% plotting the points the rays pass through
        if vis == 1
            hS = plotPoints(hS,xMid,yMid,idx);
        end
        
    end
    
    
end

end