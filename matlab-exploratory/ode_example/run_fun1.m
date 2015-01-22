% run with fun1

 [tv1 f1]=ode23('fun1',[0 5],1);
 [tv2 f2]=ode45('fun1',[0 5],1); 
 plot(tv1,f1,'-.',tv2,f2,'--') 
 title('y''=-ty/sqrt(2-y^2), y(0)=1, t in [0, 5]') 
 grid 
 axis([0 5 0 1]) 