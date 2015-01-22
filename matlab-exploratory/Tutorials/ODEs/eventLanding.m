function [value,isTerminal,dir] = eventLanding(~,z)

% Event occurs when value == 0
value = z(3);

% Terminate solution on event if true
isTerminal = true;

% Direction of events to consider
dir = 0;
