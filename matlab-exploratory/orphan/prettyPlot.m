function prettyPlot(varargin) 
    if (nargin >= 1); tl = varargin{1}; else tl = 0.01; end
    if (nargin == 2); mt = varargin{2}; else mt = 'off'; end
%     set(gcf, 'Units', 'points');
    % tick length is specified in normalized units relative to the longest
    % axis 
%     nn = max([max(xlim) max(ylim)]);
%     tl = 0.1 / nn;
    set(gca, ...
            'Box'         , 'off'     , ...
            'TickDir'     , 'out'     , ...
            'LineWidth'   , 1         , ...
            'XMinorTick'  , mt      , ...
            'YMinorTick'  , mt      , ...
            'TickLength'  , [tl tl] );
    % fix up the legend, get the handle
    lh = findobj(gcf,'Type','axes','Tag','legend');
    if (lh)
        legend boxoff;
        set(lh, 'Location', 'Best');
    end
    
    %     set(gcf, 'Units', 'normalized');