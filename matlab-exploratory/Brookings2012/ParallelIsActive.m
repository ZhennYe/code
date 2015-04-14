function isPar = ParallelIsActive()
% return true if the parallel environment is being used
try
  if matlabpool('size') > 0
    isPar = true;
  else
    % The above test fails if run by a parallel client, so test for that.
    % The parallel client is created by remoteParallelFunction, so it should
    %   be the first function on the stack:
    funcStack = dbstack();
    if strcmp(funcStack(end).name, 'remoteParallelFunction')
      isPar = true;
    else
      isPar = false;
    end
  end
catch exception
  if strcmp(exception.identifier, 'MATLAB:UndefinedFunction') && ...
     ~isempty(strfind(exception.message, 'matlabpool'))
    % parallel toolkit isn't installed, so parallel isn't active
    isPar = false;
  else
    % some other really weird error.  Nothing to do but barf it up again
    rethrow(exception)
  end
end
return