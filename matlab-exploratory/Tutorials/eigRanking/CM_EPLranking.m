%% Applying Eigenvalue Decomposition to English Premier League Scores
% Ranking of teams in a soccer league is commonly done using a point system
% based on wins, losses, and ties. In this example we apply an alternative
% ranking method using eigenvalue decomposition to rank soccer teams based
% on their goal difference and relative strength.
%
% We see how this alternative method reveals a slightly different ranking
% and identify some of the contributing factors.


%% Load and Process English Premier League Scores for 2009
clc
load EPL09

% Compose goal difference matrix
x2 = triu(x) - tril(x)'; % calculate aggregate goal difference from home and away games
GD = x2 - x2'; % create reciprocating goal difference matrix

% Apply function to calculate raw votes matrix
A = 1./(1+exp(-GD));
A(1:21:end) = 0; % zero out the diagonal

% Normalize Columns
A = bsxfun(@rdivide, A, sum(A));


%% Using EIG to Obtain the Eigenvalue Decomposition

[v,d] = eig(A);

% Extract first eigenvector and normalize to obtain team ranking score
r = v(:,1);
r = r/sum(r);

% Sort rankings in order based on score
[r_s,idx] = sort(r,'descend');

% Display teams in order of ranking
disp('Teams in order of ranking:')
disp([num2cell(1:20)', teams(idx), num2cell(r_s)])
disp(' ')


%% Comparing Manchester City's Actual Ranking to Ranking Calculated with Eigenvalue Decomposition
% Manchester City's actual ranking is 5th.  However from our compuations
% they came out 2nd.
%
% Looking at their aggregate goal difference track record, we see that they
% beat the strongest team, Chelsea, by a large margin.

% Extract and sort Manchester City's aggregate goal difference
mcity = -GD(:,12);
[mcity_s,mcity_i] = sort(mcity,'descend');

% Display Results
disp('Manchester City Aggregate Goal Difference:')
disp([teams(mcity_i), num2cell(mcity_s)])
disp(' ')


%% Comparing Tottenham Hotspur's Actual Ranking to Ranking Calculated with Eigenvalue Decomposition
% Tottenham Hotspur's actual ranking is 4th.  However from our compuations
% they came out 7th.
%
% Looking at their aggregate goal difference track record, we see that they
% had less wins and a smaller aggregate goal difference.  Note that because
% of the exponential function, the 11-0 aggregate goal difference against
% Wigan Athletic (which is appears to be a weak team) does not increase their
% ranking significantly.

% Extract and sort Tottenham Hotspur's aggregate goal difference
spurs = -GD(:,17);
[spurs_s,spurs_i] = sort(spurs,'descend');

% Display Results
disp('Tottenham Hotspur Aggregate Goal Difference:')
disp([teams(spurs_i), num2cell(spurs_s)])
