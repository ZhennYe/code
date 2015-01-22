%% real fourier transform coefficients for square pulse wave


clear , close all;
 Fs=60;
 t=0:1/Fs:20-1/Fs;
 y=square(t,50);
 y(y>0)=2;
 y(y<0)=-12;
 figure, plot(t,y);
 axis ([0 20 -20 10])
 % Fourier Series 
 a0=0;
 Fy=zeros(size(t));
 N=100;
 for n=1:2:N
      Fy=Fy+(4/n*pi)*sin(2*pi*n*t/(2*pi));
 end
 hold on,
 plot(t,Fy,'r')
 legend(' Square ','Fourier Approx');
 
 