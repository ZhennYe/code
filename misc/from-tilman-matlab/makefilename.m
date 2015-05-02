% Make a data file name
% Assumes data directory as below
function f = makefilename(day, fnum, dir, fprefix)
    data_location = 'C:/Users/tilman/Documents/data';
    fext = 'h5';
    f = sprintf('%s/%s/%s/%s_%s_%s.%s', ...
        data_location, dir, day, fprefix, day, fnum, fext);
