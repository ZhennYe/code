%  fitsmooth.m
% 
%  The electricity consumption data is loaded from USElectricityInterp.mat
%  into MATLAB.  It contains two vectors, the time and the amount of
%  electricity consumed.  The vector containing the electric consumption
%  data is sent to NPTMOVAVG which smooths the data using convolution.
%  Then, a polynomial is fitted to the smoothed data to give the final
%  parameter values for a model of the overall trend.  Periodicity effects
%  are not taken into consideration for this particular model.

%% Load and smooth data
% Load the t and R vectors which contain the time values and the amount of
% electricity consumed, respectively.
load USElectricityInterp

%  Smooth the data using convolution for 12- and 24-point moving averages
Rsm1 = nptmovavg(R,12);
Rsm2 = nptmovavg(R,24);
%  Plot the raw data with the results from the smoothing operation.
figure(1)
clf
plot(t,R,'b.-')
hold on
plot(t,Rsm1,'r','LineWidth',3)
plot(t,Rsm2,'k','LineWidth',3)
xlabel('Date'), ylabel('Retail consumption (10^9 kWh/day)')
legend('Data','1-year moving average','2-year moving average',...
    'Location','NW')

%% Perform quadratic fit
%  Locate the index values for the good data
%  The smoothed data has NaNs at the ends, so we determine the index values
%  for the data points that are not NaN
idx = ~isnan(Rsm2);
%  Create the variable, y to contain the smoothed data values
y = Rsm2(idx);
%  Create the varaible, x, which contains the good time values and shifts
%  them by 2000 in order to avoid scaling issues.
x = t(idx) - 2000;
%  Determine the coefficients of the quadratic formula which is fitted to
%  the smoothed data
c = polyfit(x,y,2)      %#ok<*NOPTS>
%  Evaluate the fitted quadratic polynomial at same locations as the
%  original data points
Rpred = polyval(c,x);
%  Plot the smoothed data and the fitted data to get a visual of how well
%  the polynomial model fits the smoothed data
figure(2)
plot(t,Rsm2,x+2000,Rpred,'LineWidth',3)
xlabel('Date'), ylabel('Retail consumption (10^9 kWh/day)')
legend('Smoothed Data','Quadratic Polynomial Fit','location','NW')

%% Compare fits of various degree polynomials
%  Locate the index values for the good data
idx = ~isnan(Rsm1);
%  Shift x variable to avoid scaling issues
x = t-2000;
%  Fit polynomials of various degree
fprintf(1,'\n Order of Polynomial  |  Error \n')
fprintf(1,'------------------------------------\n')
for k=1:9
    c = polyfit(x(idx),Rsm1(idx),k);
    
    fprintf(1,'         %d            |  %f \n',k,norm(polyval(c,x(idx))-Rsm1(idx)))
    %  Plot result
    figure(3);
    subplot(3,3,k);
    hlp = plot(t,Rsm1,t,polyval(c,x),'LineWidth',3);
    xlabel('t'), ylabel('R')
    title(['n = ',num2str(k)])
    axis tight;

    figure(4);
    clf
    ah4 = axes;
    copyobj(flipud(hlp),ah4);
    legend('Smoothed data (1-year avg.)',...
         ['Polynomial (n = ',num2str(k),') fit'],'Location','NW')
    pause(2)
end