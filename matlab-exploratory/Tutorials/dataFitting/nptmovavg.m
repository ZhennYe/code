function z = nptmovavg(y,n)
% NPTMOVAVG  n-point centered moving average
%
% y = nptmovavgfn(x,n) calculates centered moving average of x with window
% of n points.  If n is odd, then n = 2k+1, so k points are used on either
% side of the current point.  The first and last k points of y are set to 
% be NaN, because there is insufficient data to calculate an n-point 
% average.  If n is even, then n = 2k, and the window includes the k-1
% points before the current point and the k points after it.  Again, the
% first k-1 points and last k points of y are set to be NaN.


% Error checking; make sure that n is an integer
n1 = round(n);
if n~=n1
    warning('nptmovavg:nonIntegerN',['A noninteger value was input ',...
        'for the smoothing window.  ',num2str(n),' has been rounded',...
        ' to ',num2str(n1)])
    n = n1;
end
% Make sure n is positive; if n=1, there is no smoothing to do
if n==1
    z = y;
    return
elseif n<1
    error('Number of points must be positive')
end

% Calculate kernel
% Get window half-width
n2 = (n-1)/2;
n1 = floor(n2);
% n1==n2 if n is odd (so n2 will be n1-1, otherwise n2 = n1)
n2 = n1-(n1==n2);
% Make kernel same "shape" (row or column) as data
m = size(y,1);
if m==1
    krnl = ones(1,n)/n;
else
    krnl = ones(n,1)/n;
end
% Do the convolution
z = convn(y,krnl,'same');
% Overwrite the ends with NaNs
zlength = length(z);
% First n1 elements
z(1:n1) = NaN;
% Last n2+1 elements
z((zlength-n2):zlength) = NaN;