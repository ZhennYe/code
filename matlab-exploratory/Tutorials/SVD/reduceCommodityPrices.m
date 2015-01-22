% REDUCECOMMODITYPRICES
% Analyze the price data for six different commodities over almost two
% decades.  Find the singular value decomposition of the data.  Then, using
% Principal Component Analysis, perform model order reduction to include
% the larger singular values and remove the noise contained in the smaller
% singular values.

%% Load the commodity price data into MATLAB
load CommodityPrices

%% Visualize the original data
figure(1)
plot(dates,CPrices,'LineWidth',2)
datetick('x')
xlabel('Year')
ylabel('USD')
legend(labels,'location','eastoutside')
% set(gcf,'Position',[1000 404 1091 623])
axis tight

%% Shift and scale the original data
%  Subtract mean and divide by std dev.
colMeans = mean(CPrices);   % mean of each commodity
colDevs = std(CPrices);     % std of each commodity
% Shift the data so that it is centered about zero
CP0mean = bsxfun(@minus,CPrices,colMeans);
% Scale the data by dividing each commodity by one standard deviation
CPNorm = bsxfun(@rdivide,CP0mean,colDevs);

%% Visualize the shifted and scaled data
figure(2)
plot(dates,CPNorm,'LineWidth',2)
datetick('x')
xlabel('Year')
ylabel('Scaled Data')
legend(labels,'location','eastoutside')
% set(gcf,'Position',[2411 404 1091 623])
axis tight

%%  Shift each variable up a bit for separation, so we can see shape
figure(3)
plot(dates,bsxfun(@plus,CPNorm,0:2.5:2.5*(size(CPrices,2)-1)),'LineWidth',2)
datetick('x')
xlabel('Year')
set(gca,'YColor',0.8*ones(1,3))
legend(labels,'location','eastoutside')
% set(gcf,'Position',[2411 404 1091 623])
axis tight

%  Oil, electricity, and gold are all pretty closely correlated.
%  Cotton shares some similarities with coffee and corn.  But corn also has
%  some commonality with oil/lx/gold.


%% Do SVD and look at singular values
[U,S,V] = svd(CPNorm);
% Look at the singular values - note that the first two are large
fprintf(1,'\nThe singular values for the six commodities\n') %#ok
fprintf(1,'    %f\n',diag(S)) %#ok


%% Look at the first 10 rows of SV' (weighting of U)
%  X = USV', so we can treat each column of data (X) as a linear combination
%  of basis vectors given by the columns of U.  The linear combination that
%  gives the jth column of data is the jth column of the matrix SV'.  Let's
%  see those columns:
coeffs = S*V';

fprintf(1,'\nThe first 10 rows of the matrix SV'' \n') %#ok
fprintf(1,['    ',repmat('%9.6f    ',1,6),'\n'],coeffs(1:10,:)') %#ok

%  We can see we only have as many non-zero coefficients as singular values
%  (ie the number of columns in the original data).  But we can also see
%  that most are dominated by the first two coefficients.  The first four
%  commodities (oil/lx/gold/corn) are dominated by the first component.
%  The next two (cotton/coffee) are dominated by the second component.

%% View the first two principal components
%  From the above findings, we should expect the first two principal
%  components to provide the broad overall shape of all the commodity
%  prices.  Let's look at them and see...
figure(4)
plot(dates,U(:,1:2),'LineWidth',2)
legend('Principal Component 1','Principal Component 2','location','n')
datetick('x')
xlabel('Year')
axis tight

%  Sure enough, we recognize two basic shapes.  The first follows the price
%  of gold quite closely: a general upward parabolic arc, with a small bump 
%  around month 50, then a big spike around 210.  The second looks more
%  like the price of cotton: a big spike around 50, followed by a steady
%  decline, before leveling off (but with some fluctuation) around 130.

%% Reproduce data from some subset of the principal components
%  From above, we expect the prices to be mostly reproducible using just
%  the first two components.  Let's run through models created from 1, 2,
%  3, and 4 principal components, comparing the models with the original
%  data
for k=1:6
    % k = number of principal components
    figure(k+4)
    %  Loop through the various commodities
    for j=1:size(CPrices,2)
        subplot(2,3,j)
        %  k-component model
        reducedmodel = U(:,1:k)*coeffs(1:k,j);
        %  Plot model and data
        plot(dates,CPNorm(:,j),dates,reducedmodel,'LineWidth',2)
        datetick('x')
        axis tight
        title({labels{j},[num2str(k),'-component model']})
    end
%     set(gcf,'Position',[2411 404 1091 623])
    pause
end