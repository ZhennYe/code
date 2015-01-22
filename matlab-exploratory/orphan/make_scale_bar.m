% Automatically adds scale bar to active plot
% Currently does not take inputs yet.
% Error bars are 20% of scale, text is added to note length of each segment
function make_scale_bar(varargin)

    % compute the coordinates of the scale bar
    xx = xlim; yy = ylim;
    xlen = xx(2) - xx(1);  
    ylen = yy(2) - yy(1);
    xbarlen = round(xlen * 0.1);
    ybarlen = round(ylen * 0.2);
    xbarpos = xx(1) + round(xlen * 0.6);
    ybarpos = yy(1) + round(ylen * 0.6);
    xbar = [xbarpos, xbarpos + xbarlen, ... 
            ybarpos, ybarpos];
    ybar = [xbarpos, xbarpos, ... 
            ybarpos, ybarpos + ybarlen];
        
    % make sure hold is on
    hold_state = ishold;
    if(~hold_state); hold on; end
    
    % plot the scale bar
    plot(xbar(1:2), xbar(3:4), 'k');
    plot(ybar(1:2), ybar(3:4), 'k');
    text(xbar(1), xbar(3) - ylen * 0.1, num2str(xbarlen));
    text(xbar(1), ybar(4) + ylen * 0.1, num2str(ybarlen))
    
    % turn hold back off if thats how we found things
    if(~hold_state); hold off; end