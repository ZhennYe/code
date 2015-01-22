clear,clc

load intSurfPlane;

x = InstMesh.Vertices(1,:)'; 
y = InstMesh.Vertices(2,:)'; 
z = InstMesh.Vertices(3,:)';

p0=[0 0 0]; v=[1 0 0];

% plane=createPlane(p0,v); % createPlane from geom3d toolbox 
[ A, B, C, D ] = plane_normal2imp_3d ( p0, v ); % create implicit plane function 
segment_start=nan(3,round(size(InstMesh.Faces,2)/2)); 
segment_finish=segment_start; 
count=1; 
for s=1:size(InstMesh.Faces,2) 
t(:,1)=InstMesh.Vertices(:,InstMesh.Faces(1,s)); 
t(:,2)=InstMesh.Vertices(:,InstMesh.Faces(2,s)); 
t(:,3)=InstMesh.Vertices(:,InstMesh.Faces(3,s)); 
[ num_int, pi ] = plane_imp_triangle_int_3d ( A, B, C, D, t ); 
if num_int==2 
segment_start(:,count)=pi(:,1); 
segment_finish(:,count)=pi(:,2); 
count=count+1; 
% hold on,plot3(pi(1,:),pi(2,:),pi(3,:)) 
end 
end 
segment_start(:,all(isnan(segment_start),1))=[]; % remove unused poritons 
segment_finish(:,all(isnan(segment_finish),1))=[]; % remove unused poritons

thr=2; nol=1; 
while ~isempty(segment_start) 
lin{nol}=[segment_start(:,1) segment_finish(:,1)]; 
segment_start(:,1)=[]; 
segment_finish(:,1)=[]; 
while 1 
testDistStart1=sum((lin{nol}(1,end)-segment_start(1,:)).^2 + ... 
(lin{nol}(2,end)-segment_start(2,:)).^2 + ... 
(lin{nol}(3,end)-segment_start(3,:)).^2,1); 
testDistStart2=sum((lin{nol}(1,end)-segment_finish(1,:)).^2 + ... 
(lin{nol}(2,end)-segment_finish(2,:)).^2 + ... 
(lin{nol}(3,end)-segment_finish(3,:)).^2,1); 
[minDist1, best_ind1]=min(testDistStart1); 
[minDist2, best_ind2]=min(testDistStart2); 
if minDist1<thr || minDist2<thr 
if minDist1<minDist2 
lin{nol}=[lin{nol} segment_finish(:,best_ind1)]; 
segment_start(:,best_ind1)=[]; 
segment_finish(:,best_ind1)=[]; 
else 
lin{nol}=[lin{nol} segment_start(:,best_ind2)]; 
segment_start(:,best_ind2)=[]; 
segment_finish(:,best_ind2)=[]; 
end 
else 
nol=nol+1; 
break 
end 
end 
end

s = trisurf(InstMesh.Faces', x, y, z, 'EdgeColor', 'none', 'FaceColor', 'interp'); 
daspect([1 1 1]); 
for m=1:size(lin) 
hold on,plot3(lin{m}(1,:),lin{m}(2,:),lin{m}(3,:)) 
end 
hold off
