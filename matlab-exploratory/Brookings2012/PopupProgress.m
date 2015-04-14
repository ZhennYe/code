function PopupProgress(name, totalTicks, varargin)
% PopupProgress(name, totalTicks, updatePeriod, keepUnderscores)
% Create or update a progress indicator.
% Note: in a parallel environment, uses a temporary file
%  -To create the progress indicator, pass the name, the total number of
%   ticks (updates to be completed) and if desired, specify the period to
%   update estimated completion time.
%  -To increment progress, pass only the name of an existing indicator.
%   The indicator automatically closes if progress is complete.
%  INPUTS:
%    name:  string, describing the progress indicator
%   OPTIONAL:
%    totalTicks:  the total number of progress updates until completion
%    updatePeriod:  interval (in seconds) between updated estimates of
%                   completion time.  Defaults to 5s.
%    keepUnderscores:  interpret underscores as characters, not subscripts.
%                      Defaults to true

if nargin == 1
  % increment progress bar
  incrementPopupTicks(name);
else
  % get options
  defaultOptions = { ...
    'updatePeriod', 5, ...
    'keepUnderscores', true, ...
    'errorOnPreexisting', true ...
    };
  options = GetOptions(defaultOptions, varargin);
  makePopupTicks(name, totalTicks, options)
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function makePopupTicks(name, totalTicks, options)
% Create a progress bar that can be updated with future calls
% 1. Progress (number of ticks) is stored in a temporary file.
% 2. A figure is created with
%   a) a waitbar
%   b) a timer
%   c) a callback function actived by the timer, which monitors the
%      temporary file and updates the waitbar appropriately

invalidChars = unique(name(regexp(name, '[^ .a-zA-Z_0-9\-]')));
if any(invalidChars)
  error('PopupProgress name ''%s'' contains invalid character(s): %s', ...
        name, invalidChars)
end
if any(findobj('Name', name))
  % There is already a progress bar with this name
  
  % delete the temporary file holding the tick count
  delete([tempdir, name, '_matlabTimer.txt'])
  
  if options.errorOnPreexisting
    error('There is already a progress bar named ''%s''', name)
  else
    % just close the old progress bar and continue
    close(findobj('Name', name));
  end
end

% store the tick count in a temporary file
if options.keepUnderscores
  update.name = realUnderscores(name);
else
  update.name = name;
end
update.fileName = [tempdir, name, '_matlabTimer.txt'];
fid = fopen(update.fileName, 'w');
if fid < 0
  error('Couldn''t create temporary file: %s', update.fileName)
end
% close the file, leaving it empty
fclose(fid);

% create the waitbar figure
tickStr = sprintf('%lu / %lu', 0, totalTicks);
message = {update.name, tickStr, ''};
update.totalTicks = totalTicks;
update.updatePeriod = options.updatePeriod;
update.t0 = tic;
h = waitbar(0, message, 'Name', name);
set(h, 'HandleVisibility', 'on')
hAxes = get(h, 'CurrentAxes');
set(hAxes, 'FontName', 'Arial', 'FontSize', 14)
update.waitbarHandle = h;

% create the timer and set the callback functions
%   the callback function checks the temporary file and updates the waitbar
%   appropriately. Once the last tick is called, everything closes
% update structure is stored in the timer's UserData
t = timer('tag', 'progresstimer', ...
  'name', name, ...
  'timerfcn', @popupCallback, ...
  'stopfcn', @popupStopCallback, ...
  'period', update.updatePeriod, ...
  'StartDelay', update.updatePeriod, ...
  'ExecutionMode', 'FixedDelay', ...
  'UserData', update);

% start the timer
start(t);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function incrementPopupTicks(name)
% Increment the ticks in the progress bar

% Increment ticks using a temporary file
% This method works even in a parallel environment (e.g. with parfor)
fileName = [tempdir, name, '_matlabTimer.txt'];
fid = fopen(fileName, 'a');
if fid < 0
  error('couldn''t append to file %s', fileName)
end
fwrite(fid, ' ', 'char*1');
fclose(fid);
return




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function popupCallback(obj, event, string_arg) %#ok<INUSD>
% monitor the progress via the temporary file, and update the progress bar
% appropriately

% get the update structure from the timer's (obj) UserData
update = get(obj, 'UserData');

% get the number of ticks, as the length of the file
fid = fopen(update.fileName, 'r');
if fid < 0
  error('Couldn''t open file %s', update.fileName)
end
fseek(fid, 0, 'eof');
ticks = ftell(fid);
fclose(fid);

if ticks == 0
  % no progress, wait for next check
  return
end

% update the tickStr
tickStr = ...
  sprintf('%lu / %lu', ticks, update.totalTicks);
% calculate the length of the bar on the waitbar (the fraction complete)
frac = ticks / update.totalTicks;

% check the total elapsed time
tNow = toc(update.t0);
% estimate the remaining time
tFinish = (update.totalTicks - ticks ) * tNow / ticks;
timeRemaining = [getTimeString(tFinish), ' remaining'];
message = {update.name, tickStr, timeRemaining};

% update the waitbar
if ishandle(update.waitbarHandle)
  % check to make sure the waitbar still exists, preventing a warning if
  % the progress bar is deleted by an external routine
  waitbar(frac, update.waitbarHandle, message);
end

if ticks >= update.totalTicks
  % progress is complete, stop the timer
  stop(obj)
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function popupStopCallback(obj, event, string_arg) %#ok<INUSD>
% shutdown the progress bar

% get the update structure
update = get(obj, 'UserData');

% delete the timer
delete(obj)

% close the waitbar
if ishandle(update.waitbarHandle)
  close(update.waitbarHandle);
end

% delete the temporary file
if exist(update.fileName, 'file')
  delete(update.fileName);
end

return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function timeString = getTimeString(tFinish)
% convert a time in seconds into a human-friendly string
s = round(tFinish);
if s < 60
  timeString = sprintf('%ds', s);
  return
end
m = floor(s/60);
s = s - m * 60;
if m < 60
  timeString = sprintf('%dm%ds', m, s);
  return
end
h = floor(m/60);
m = m - h * 60;
if h < 24
  timeString = sprintf('%dh%dm%ds', h, m, s);
  return
end

d = floor(h/24);
h = h - d * 24;
timeString = sprintf('%gd%dh%dm%ds', d, h, m, s);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function texSafeString = realUnderscores(titleStr)
%  Protects the underscores in titleStr from being interpreted as
%  subscript commands by inserting a '\' before them.

ind = strfind(titleStr, '_');
texSafeString = '';
previous = 1;
for n = 1:length(ind)
  if ind(n) > 1
    texSafeString = [texSafeString, titleStr(previous:(ind(n)-1))]; %#ok<*AGROW>
  end
  texSafeString = [texSafeString, '\'];
  previous = ind(n);
end
texSafeString = [texSafeString, titleStr(previous:end)];
return