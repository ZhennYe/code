function x = getParam(params, p)
    index = find(ismember(params.parameterName, p));
    if (~isempty(index)) 
        % its the second index here becasue the first parameter
        % says what time point that parameter was set (generally 0)
        x = params.values{index}{2};  
    else
        x = 'xxx';
        fprintf('Warning: RTXI Parameter: "%s" not found.\n', p);
    end