% This is how I like my plots to look.
% Bigger fonts, and to take up most of the screen.
% Based on version from Mike Economo
function set_plot_props()
    set(0, 'DefaultFigureUnits', 'Normalized');
%     set(0, 'DefaultFigurePosition', [.025, .085, .75, .8]); % Whole Screen
    set(0, 'DefaultFigurePosition', [.025, .085, .95, .8]); % Whole Screen
%     set(0, 'DefaultFigurePosition', [1.0244 0.0817 0.9500 0.8000]); % right monitor
    set(0, 'DefaultLineMarkerSize',18);
    set(0, 'DefaultAxesFontSize',12);
    set(0, 'DefaultLineLineWidth', 3);
    set(0, 'DefaultLineColor', [0.847,0.161,0.0]);