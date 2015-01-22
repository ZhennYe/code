function Spike = GetSpikeTimesExperimental(t, V, options)
% Spike = GetSpikeTimesExperimental(t, V, options)
% Find spikes as traces that travel clockwise around a fixed point
%  on a graph of V vs dV/dt
% This isn't really a stand-alone function.  It's something that's
% meant to be plugged into AnalyzeWaveform.m
% It is probably actually a bad idea to begin with.

DebugPlots = true;

%Calculate derivatives as needed
dt = t(2) - t(1);
[deriv, deriv2] = PolyDeriv(V, dt, 7, 3);

%High-pass filter voltage for this analysis
OldV = V;
SmoothTime = 20;  %ms
if t(2) - t(1) < SmoothTime
  n = 2;
  while t(n) - t(1) < SmoothTime
    n = n + 2;
  end
  [B,A] = butter(2, 2 / n,'high');
  % [B,A] = besself(2, 2/n, 'high');
  V = filtfilt(B, A, OldV);
end


[VMid, dVMid] = getInteriorPoint(V, deriv);
fprintf('(VMid=%g,dVMid=%g)\n', VMid, dVMid)

[n1List, n2List, BadCount] = getIndLists(V, deriv, VMid, dVMid, options);

DoRecurse = true;
while DoRecurse
  [n1List2, n2List2, BadCount2, VMid2, dVMid2] ...
      = RecurseGetSpikes(V, deriv, n1List, n2List, BadCount, ...
			 options);
  fprintf('BadCount1=%g, Num1=%g, BadCount2=%g, Num2=%g\n', ...
	  BadCount, length(n1List), BadCount2, length(n1List2))
  if BadCount > .2 * length(n1List)
    if BadCount2 > .2 * length(n1List2)
      n1List = [];
      n2List = [];
      DoRecurse = false;
    else
      n1List = n1List2;
      n2List = n2List2;
      VMid = VMid2;
      dVMid = dVMid2;
      DoRecurse = true;
      BadCount = BadCount2;
    end
  elseif BadCount2 <= .2 * length(n1List2)
    if (BadCount2 / length(n1List2) < BadCount / length(n1List)) || ...
       (BadCount2 == 0 && length(n1List2) > length(n1List))
      n1List = n1List2;
      n2List = n2List2;
      VMid = VMid2;
      dVMid = dVMid2;
      DoRecurse = true;
      BadCount = BadCount2;
    else
      DoRecurse = false;
    end
  else
    DoRecurse = false;
  end
end

%now spikes are all bracketed between n1List and n2List

%Get spike shape (either dummy struct, or filled with data)
Spike = getSpikeShape(n1List, n2List, t, OldV, deriv, deriv2, NoShape);

if needPlot(options) && DebugPlots
  plotGetSpikeTimes(t, V, deriv, VMid, dVMid, options);
end

if isempty(Spike.Times)
  Spike.Intervals = [];
  Spike.Frequencies = [];
else
  Spike.Intervals = Spike.Times(2:end) - Spike.Times(1:(end-1));
  Spike.Frequencies = 1000 ./ Spike.Intervals;
end
return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [VMid, dVMid] = getInteriorPoint(V, deriv)
[deriv, sortInd] = sort(deriv);
n = length(deriv);
while(deriv(n) > 1.1 * deriv(n-1))
  n = n - 1;
  if(n == 1)
      n = length(deriv);
      break
  end
end
v_at_dVMax = V(sortInd(n));
dVMax = deriv(n);
dVMid = dVMax / 3.0;
if(dVMid < 1.0)
  dVMid = 1.0;
end

V = sort(V);
n = length(V);
while(V(n) > 1.1 * V(n-1))
  n = n - 1;
  if(n == 1)
      n = length(V);
      break
  end
end
VMid = (V(n) + v_at_dVMax) / 2.0;

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Second try, maybe a few weird points screwed-up spike detection
function [n1List, n2List, BadCount, VMid, dVMid] ...
    = RecurseGetSpikes(V, deriv, n1List, n2List, BadCount, options)
numV = length(V);
Cutoff = .1 * numV;
if length(n1List) > Cutoff || BadCount > Cutoff
  n1List = [];
  n2List = [];
  VMid = Inf;
  dVMid = Inf;
  BadCount = Inf;
  return
end
excludeInd = [];
for Ind = 1:length(n1List)
  n1 = n1List(Ind);
  n2 = n2List(Ind);
  excludeInd = [excludeInd, (n1+1):(n2-1)];
end
GoodInd = setdiff(1:numV, excludeInd);
if length(GoodInd) < 10
  n1List = [];
  n2List = [];
  VMid = Inf;
  dVMid = Inf;
  BadCount = Inf;
  return
end  

fprintf('(MaxV=%g,MaxdV=%g)\n', max(V(GoodInd)), max(deriv(GoodInd)))
[VMid, dVMid] = GetInteriorPoint(V(GoodInd), deriv(GoodInd));
fprintf('(VMid=%g,dVMid=%g)\n', VMid, dVMid)

[n1List, n2List, BadCount] = GetIndLists(V, deriv, VMid, dVMid, options);
return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [n1List, n2List, BadCount] = getIndLists(V, deriv, VMid, dVMid, ...
						  options)
%Loop through data, looking for spikes
numV = length(V);
n1List = [];
n2List = [];
n = 3;
BadCount = 0;  %BadCount keeps track of the number of times V gets
               %large without ever achieving a large derivative.
               %If BadCount is large, it means there are no spikes,
               %just random voltage fluctuations
while n <= numV
  if V(n) < VMid
    n = n + 1;
  else
    if deriv(n) > dVMid
      n1 = n - 1;
    elseif dVMid < deriv(n-1) + ...
	  (VMid - V(n-1)) * (deriv(n) - deriv(n-1)) / (V(n) - V(n-1))
      n1 = n - 2;
    else  %Probably on a bad trajectory
      n1 = n - 1;
      n = n + 1;
      if n >= numV
        BadCount = BadCount + 1;
        break;
      end
      while V(n) >= VMid & deriv(n) <= dVMid
	n = n + 1;
	if n >= numV
	  BadCount = BadCount + 1;
	  break
	end
      end
      if n >= numV
	break
      end
      if V(n) < VMid
        BadCount = BadCount + 1;
        n = n + 1;
        continue
      end
    end

    n2 = n + 1;

    while n2 <= numV
      if V(n2) > VMid
    	n2 = n2 + 1;
      else
	break
      end
    end
    if n2 > numV
      break
    end
    
    %We've bracketed a spike between n1 and n2
    
    %Extend out n1 and n2 to encompass time before and after spike to:
    %  a)obtain spike shape info
    %  b)remove large spikes and look for smaller ones
    %  c)maintain consistency with different options
    if n1 - 2 >= 1
      while deriv(n1) > 0 || deriv(n1 - 1) > 0 || deriv(n1 - 2) > 0
	if(n1 - 2 <= 1)
	  n1 = 1;
	  break;
	else
	  n1 = n1 - 1;
	end
      end
    end
    n1List = [n1List, n1];
    
    if n2 + 2 <= numV
      while deriv(n2) < 0 || deriv(n2 + 1) < 0 || deriv(n2 + 2) < 0
	if(n2 + 2 >= numV)
	  n2 = numV;
	  break;
	else
	  n2 = n2 + 1;
	end
      end
    end
    n = n2;
    n2List = [n2List, n2];
    if options.firstOnly
      break
    end
  end
end
return