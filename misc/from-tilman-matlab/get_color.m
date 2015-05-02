% make color gradient from blue to red
% i: which element in the color list you want
% len: the number of different colors you want
% returns RGB triplet for color
% usage: plot(x, y, 'Color', get_color(i, length(trials)));
function c = get_color(i, len)
    rgbvals = linspace(0, 255, len);
    c = [rgbvals(i) / 255, 0, rgbvals(end-(i-1)) / 255];
  