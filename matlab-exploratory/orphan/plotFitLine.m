% Fit a line and plot it on the current axis.
% Make sure the calling code has 'hold on'
% args:
%    x: x data to fit over
%    y: y data to fit
%    varargin{1}: line style
%    varargin{2}: will not plot if arg is present
% returns:
%    r: r value of the fit
%    s: slope of the line
%    h: handle to the line
function [r,s,h] = plotFitLine(x, y, varargin)

% option to plot line with alternate style
if (nargin >= 3); linestyle = varargin{1}; else linestyle = 'k--';end
% option to actually plot the line or just to compute it
if (nargin == 4); doplot = 0; else doplot = 1; end

% plot(x, y, 'kx');  % test
% make sure there are enough points to fit a line
if (length(x) < 2 || length(y) < 2)
    fprintf('\t%s: Too few points for line fit: len(x) = %d, len(y) = %d\n', ...
        mfilename(), length(x), length(y));
    r = 0; s = 0; h = 0;
    return;
end
coeff = polyfit(x, y, 1);
values = polyval(coeff, x);
linefit = corrcoef(y, values);
if (doplot);  h = plot(x, values, linestyle); end
r = linefit(1,2);
s = coeff(1);