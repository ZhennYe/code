% produce a nice output file from current figure
% inputs:
%   p: path to save file, Windows desktop by default
%   format: type of output file, depends on file types supported by 
%           export_fig, PDF by default
% Example:
% To get png: makepdf('C:\Users\tilman\Desktop\dataplots', 'png');
function makepdf(varargin)
    if(nargin == 0)
        p = 'C:\Users\tilman\Desktop\dataplots';
        format = 'pdf';
    elseif (nargin == 1)
        p = varargin{1};
        format = 'pdf';
    elseif (nargin == 2)
        p = varargin{1};
        format = varargin{2};
    end
    
    % my way, generates PDF that is slightly reformatted
    % orient landscape
    % t = regexprep(get(gcf,'name'), ' ', '_');
    % s = sprintf('%s/%s_%s.pdf',p, t, datestr(now,'mmm_dd_yy_HHMMSS'));
    % print('-dpdf', s)

    % gives a PDF that looks exactly like on the screen (with white
    % background)
    % uses 'export_fig' from file exchange
    addpath('../util/export_fig');
    set(gcf, 'color', 'white');
    set(gca, 'FontName', 'Arial');
    
    % get rid of spaces in filename
    t = regexprep(get(gcf,'name'), ' ', '_');
    
    % make today's directory
    todaydir = sprintf('%s/%s', p, makedirstr());
    if(~exist(todaydir, 'dir'))
        mkdir(todaydir);
    end
    
    % print the figure
    s = sprintf('%s/%s_%s.%s', todaydir, t, datestr(now,'yyyy-mm-ddTHHMMSS'), format);  % ISO date format
    export_fig(s, sprintf('-%s', format), '-nocrop');
    

% generate name of today's directory
function s = makedirstr()
    s = sprintf('%s', datestr(now,'yyyy-mm-dd'));