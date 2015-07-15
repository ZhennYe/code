function shollColor(shollfile)
% Usage: shollColor(shollfile, somapos)
% Input:
%   shollfile is an N x 4 space-separated (x,y,z,color) txt file.
%   somapos is (x,y,z) of the middle of the soma, or a good estimation
%
% This function will plot a colorized version of the sholl diagram. Best if
% used after function sholl_color in neuron_getProperties.py; about
% 200,000 points are required for a pretty figure (keep decreasing
% interdist until you get this many); shouldn't take very long to get the
% points or plot it.


% Inputs
if isstr(shollfile) ~= 1
  m = size(shollfile);
  if m(2) ~= 4
    disp('The sholl file must either be a string for a file (Nx4, txt)')
    disp(' or an Nx4 (x,y,z,col) matrix')
  else
    sholl = shollfile;
  end
else
  sholl = importdata(shollfile);
end
%m = size(somapos);
%if m(1) ~= 3 && m(1) ~= 1
%  disp('Somapos ,must be a 3-tuple (x,y,z) or (x;y;z)')
%end


% Plot
scatter3(sholl(:,1), sholl(:,2), sholl(:,3), 1, sholl(:,4))
hold on;
% Soma
[~, ind] = min(sholl(:,4));
scatter3(sholl(ind,1), sholl(ind,2), sholl(ind,3), 300, 0, 'filled')
axis('equal');
%set(gca, 'xtick', []);
%set(gca, 'ztick', []);
%set(gca, 'ytick', []);
set(gcf, 'color', 'w');
set(gca, 'visible','off');


end

