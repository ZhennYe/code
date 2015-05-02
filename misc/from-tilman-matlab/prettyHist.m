% Make histogram bars have a custom color for the edges and faces
% of each bar.
% Inputs:
%   fc: string input for face color (eg. 'r')
%   ec: string input for edge color (eg. 'g')
%   hx: handle to object to operate on
function prettyHist(fc, ec,varargin)
    if (nargin == 3)
        hx = varargin{1};
    else
        hx = findobj(gca,'Type','patch');
    end
    set(hx,'FaceColor',fc,'EdgeColor',ec);
    