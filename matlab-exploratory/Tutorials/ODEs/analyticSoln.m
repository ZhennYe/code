function v = analyticSoln(t)
v0 = -5700; % initial velocity [m/s]
D = 3.3e-5; % drag coefficient
g = 3.72;   % gravity [m/s^2]

srt = sqrt(g/D);
v = srt*(1+(v0-srt)/(v0+srt)*exp(2*t*sqrt(g*D)))./...
    (1-(v0-srt)/(v0+srt)*exp(2*t*sqrt(g*D)));