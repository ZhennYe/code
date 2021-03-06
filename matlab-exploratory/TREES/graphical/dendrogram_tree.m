% DENDROGRAM_TREE   plots a dendrogram of a tree.
% (trees package)
%
% HP = dendrogram_tree (intree, diam, yvec, color, DD, wscale, options)
% ---------------------------------------------------------------------
%
% plots a dendrogram of the topology of a tree (must be BCT conform).
% (consider applying repair_tree first)
%
% Input
% -----
% - intree::integer:index of tree in trees or structured tree
% - diam::vector/single value: attributes to each element a horizontal
%     width {DEFAULT 0.5}
% - yvec ::vertical vector: attributes to each element a Y-position
%     {DEFAULT: metric path length}, might consider branch order instead
% - color::RGB 3-tupel, vector or matrix: RGB values {DEFAULT [0 0 0]}
%     if vector then values are treated in colormap (must be Nx1!,
%     works only with '-p' option)
%     if matrix (num x 3) then individual colors are mapped to each element
% - DD:: X XY-tupel or XYZ-tupel: coordinates offset {DEFAULT [0,0,0]}
% - wscale::scalar: spacing of terminals [default is 1].
% - options::string: {DEFAULT: ''}
%     '-p' : drawn as patches instead of lines (color then be Nx1 vector)
%     '-v' : no horizontal lines
%
% Output
% ------
% - HP::handles: depending on options HP links to the graphical objects.
%
% Example
% -------
% dendrogram_tree (sample_tree)
%
% See also xdend_tree BCT_tree
% Uses xdend_tree ver_tree dA
%
% the TREES toolbox: edit, visualize and analyze neuronal trees
% Copyright (C) 2009  Hermann Cuntz

function HP = dendrogram_tree (intree, diam, yvec, color, DD, wscale, options)

% trees : contains the tree structures in the trees package
global trees

if (nargin < 1)||isempty(intree),
    intree = length (trees); % {DEFAULT tree: last tree in trees cell array}
end;

ver_tree (intree); % verify that input is a tree structure

% use only directed adjacency for this function
if ~isstruct (intree),
    dA = trees {intree}.dA;
else
    dA = intree.dA;
end

if (nargin<2)||isempty(diam),
    diam = 0.5; % {DEFAULT: half of distance between two end-nodes}
end

if (nargin<3)||isempty(yvec),
    yvec = Pvec_tree (intree); % {DEFAULT vector: metric path length}
end

if (nargin<4)||isempty(color),
    color = [0 0 0]; % {DEFAULT color: black}
end

if (nargin<5)||isempty(DD),
    DD = [0 0 0]; % {DEFAULT 3-tupel: no spatial displacement from the root}
end
if length(DD)<3,
    DD = [DD zeros(1, 3 - length (DD))]; % append 3-tupel with zeros
end

if (nargin<6)||isempty(wscale),
    wscale = 1; % {DEFAULT: 1 um between two terminal nodes}
end

if (nargin<7)||isempty(options),
    options = ''; % {DEFAULT: no option}
end

% get the x-positions for the dendrogram:
xdend = xdend_tree (intree);

% xdend (idpar) is the same as dA*xdend
idpar = dA * (1 : size (dA, 1))'; % vector containing index to direct parent
idpar (idpar == 0) = 1;
X1 = ((xdend (idpar)) .* wscale) + DD (1); % coordinates of the nodes in dendrogram
X2 = (xdend .* wscale)           + DD (1);
Y1 = yvec (idpar)                + DD (2);
Y2 = yvec                        + DD (2);
Z1 = zeros (size (X1, 1), 1)     + DD (3);
Z2 = zeros (size (X1, 1), 1)     + DD (3);

Diam = ones (size (X1, 1), 1) .* diam;
if isempty (strfind (options, '-v')),
    % separate in horizontal and vertical components:
    X1 = [X1; X2];
    X2 = [X2; X2];
    Y2 = [Y1; Y2];
    Y1 = [Y1; Y1];
    Z1 = [Z1; Z2];
    Z2 = [Z2; Z2];
    if size (color, 1) > 1,
         color = [color; color]';
    end
    Diam = [Diam*(max (Y1) - min (Y1))/(max (X1) - min (X1)); Diam];
else
    if size (color, 1) > 1,
         color = color';
    end
end

if isempty (strfind (options, '-p')), % as lines:
    HP = line ([X1 X2]', [Y1 Y2]', [Z1 Z2]');
    set (HP, 'linewidth', diam, 'color', color);
else % as patches:
    warning ('off', 'MATLAB:divideByZero');
    A = [X2-X1 Y2-Y1] ./ repmat (sqrt ((X2-X1).^2 + (Y2-Y1).^2), 1, 2);
    warning ('on',  'MATLAB:divideByZero');
    % use rotation matrix to rotate the data
    V1 =(A*[0,-1;1,0]).*(repmat(Diam, 1, 2)./2);
    V2 =(A*[0,1;-1,0]).*(repmat(Diam, 1, 2)./2);
    HP = patch([X1+V2(:,1) X1+V1(:,1) X2+V1(:,1) X2+V2(:,1)]',...
        [Y1+V2(:,2) Y1+V1(:,2) Y2+V1(:,2) Y2+V2(:,2)]',[Z1 Z1 Z2 Z2]',color);
    set (HP, 'edgecolor', 'none');
end

