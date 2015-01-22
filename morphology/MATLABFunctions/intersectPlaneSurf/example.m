clear,clc

t=2*pi*linspace(-1/2,1/2,100).';

x=cos(t); y=sin(t); z=t;

[T N B]=myfrenet(x,y,z);
r1=.25; r2=.4; c=linspace(0,2*pi,100);
cs=cos(c); sn=sin(c);

exx=zeros(numel(x),numel(c)); eyy=exx; ezz=exx;
for s1=1:numel(x)
    a=r1.*cs;
    b=r2.*sn;
    for s2=1:numel(a)
        R=N(s1,:)*a(s2) + B(s1,:)*b(s2) + [x(s1) y(s1) z(s1)];
        exx(s1,s2)=R(1); eyy(s1,s2)=R(2); ezz(s1,s2)=R(3);
    end
end

p0=[0 0 0]; v=[0 1 0]; %a plane that parallel to the z axis and has a point [0 0 0]

intersectPlaneSurf(p0, v, exx, eyy, ezz);
