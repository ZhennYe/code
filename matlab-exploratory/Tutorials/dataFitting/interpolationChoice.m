% interpolationChoice.m
% Interpolates missing values in US Electricity consumption data


%% Load electricity consumption data
load USElectricity

% Create a complete data set that does not have the NaN values
idx = ~isnan(R);    % First, define all index values that do NOT have NaN 
                    % entries
tOK = t(idx);       % Define tOK & ROK as the given data points
ROK = R(idx);
tmiss = t(~idx);    % tmiss contains all t values that correspond to R 
                    % values which contain NaN entries

%%  Plot the raw data with marker points
% Set the beginning and end years for the data
yBeg = 1999;
yEnd = 2003;

% Create a cell array to be used for axis lables
n = length(yBeg:1/12:yEnd);
labels = cell(n,1);
for idx = 1:yEnd-yBeg+1;
    labels{1+(idx-1)*12,1} = num2str(1999+idx-1);
end

%  Plot the raw data with marker points
figure(1)
clf
hData = plot(t,R,'bo','LineWidth',3,'MarkerSize',6,'MarkerFaceColor','b');

%  Annotate the plot
xlabel('Date','FontSize',16)
ylabel({'Retail consumption';'(10^9 kWh/day)'},...
    'FontSize',16)
set(gca,'XTick',1999:1/12:2003,'XTickLabel',labels,...
    'FontSize',14,'Ytick',8:11,'TickDir','in',...
    'TickLength',[0.0300 0.0250])

% Scale and locate the plot
axis([yBeg - 0.05,yEnd - 0.53,7.6,11.4])
set(gcf,'Position',[373   225   933   483])
% set(gcf,'Position',[2295  689   933   483])
hold on

%% Perform Linear Interpolation
% Plot the linear interpolants
hLine = plot(tOK,ROK,'Color',[ 0 0.6 0 ],'LineWidth',2);

% Mark the interpolated data points
Rmiss_l = interp1(tOK,ROK,tmiss);
hLinePts = plot(tmiss,Rmiss_l,'*','Color',[0 0.6 0],'LineWidth',3,...
    'MarkerSize',11);


%% Remove the linear interpolation data from the plot
% Make the interpolant invisiable and delete the interpolated data points
set(hLine,'Visible','off')
delete(hLinePts)


%% Perform Spline Interpolation
% Define the finely spaced points at which we would like to find the
% interpolant.
t_fine = linspace(1998.9,2002.5,150);
% Define the interpolant
R_fine_spline = interp1(tOK,ROK,t_fine,'spline');
hSpline = plot(t_fine,R_fine_spline,'r','LineWidth',2);

% Mark the missing data points on the plot
Rmiss_s = interp1(tOK,ROK,tmiss,'spline');
hSplinePts = plot(tmiss,Rmiss_s,'rx','LineWidth',3,'MarkerSize',11);


%% Remove the spline interpolation data from the plot
% Make the interpolant invisiable and delete the interpolated data points
set(hSpline,'Visible','off')
delete(hSplinePts)


%% Perform PCHIP Interpolation
% Define the interpolant.
R_fine_pchip = interp1(tOK,ROK,t_fine,'pchip');
hPCHIP = plot(t_fine,R_fine_pchip,'k','LineWidth',2);

% Mark the missing data points on the plot
Rmiss_p = interp1(tOK,ROK,tmiss,'pchip');
hPCHIPpts = plot(tmiss,Rmiss_p,'k+','LineWidth',4,'MarkerSize',11);

%% Plot all of the interpolants together on one figure
delete(hPCHIPpts)
set(hLine,'Visible','on')
set(hSpline,'Visible','on')
legend('Data','Linear','Spline','PCHIP')
