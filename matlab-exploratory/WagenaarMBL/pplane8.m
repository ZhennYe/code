function output = pplane8(action,input1,input2,input3)

% pplane8  is an interactive tool for studying planar autonomous systems of
%	differential equations.  When pplane8 is executed, a pplane8 Setup
%	window is opened.  The user may enter the differential
%	equation and specify a display window using the interactive
%	controls in the Setup window.  Up to 4 parameters may also be 
%	specified.  In addition the user is give a choice of the type of 
%	field displayed and the number of field points.
%
%	When the Proceed button is pressed on the Setup window, the pplane8
%	Display window is opened, and a field of the type requested is 
%	displayed for the system.  When the mouse button is depressed in 
%	the pplane8 Display window, the solution to the system with that
%	initial condition is calculated and plotted.
%
%	Other options are available in the menus.  These are
%	fairly self explanatory.  
%
%	This is version 6.0, and will only run on version 6 of MATLAB.

%  Copywright (c)  1995, 1997, 1998, 1999, 2000, 2001, 2002, 2003 
%                  John C. Polking, Rice University
%                  Last Modified: April 24, 2003

startstr = 'pplane8';
if nargin < 1
  action ='initialize';
end



if get(0,'screendepth') == 1
   style = 'bw';
end

if strcmp(action,'initialize')
   
   % First we make sure that there is no other copy of pplane8
   % running, since this causes problems.	
   pph = findobj('tag','pplane8');
   if ~isempty(pph);
      qstring = {'There are some pplane8 figures open although they may be invisible.  ';...
            'What do you want to do?'};
      tstring = 'Only one copy of pplane8 can be open at one time.';
      b1str = 'Restart pplane8.';
      b2str = 'Just close those figures.';
      b3str = 'Do nothing.';
      answer = questdlg(qstring,tstring,b1str,b2str,b3str,b1str);
      if strcmp(answer,b1str)
         delete(pph);
         %pplane8;return
         eval(startstr);return
      elseif strcmp(answer,b2str)
         delete(pph);return
      else
         return
      end 
   end  % if ~isempty(pph);
end
   % Make sure tempdir is on the MATLABPATH.  We want to be sure that we
   % return the path to its current position when we exit.
   
   p = path;
   tmpdir = tempdir;
   ll = length(tmpdir);
   tmpdir = tmpdir(1:ll-1);
   ud.remtd = 0;
   if isempty(findstr(tmpdir,p))
     ud.remtd = 1;
     addpath(tempdir)
   end
   
   % Next we look for old files created by pplane8.
   % First in the current directory.
   
   oldfiles = dir('pptp*.m');
   ofiles = cell(0,1);
   
%    If the matrix describing the linear system has real eigenvalues that are both negative, then the
% fixed point is called a nodal sink. The classic phase portrait of a nodal sink is shown in Figure 11.2.
%   If the matrix describing the linear system has real eigenvalues that are both positive, then
% the fixed point is called a nodal source. The classic phase portrait of a nodal source is shown
% in Figure 11.3. Notice the difference in the direction of the arrows in the vector field in this
% figure.
%   If the matrix describing the linear system has imaginary eigenvalues that have negative
% real parts, then the fixed point is called a spiral sink. The classic phase portrait of a spiral
% sink is shown in Figure 11.4.
%    If the matrix describing the linear system has imaginary eigenvalues that have positive
% real parts, then the fixed point is called a spiral source. The classic phase portrait of a spiral
% source is shown in Figure 11.5.
%   These five types of equilibria are collectively known as the generic equilibria. There are
% also five nongeneric equilibria. The most important nongeneric equilibrium is called a
% center. It occurs when the eigenvalues of the m