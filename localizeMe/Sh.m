close all;
clear all;

p1 = [-10.563, 2.483];
p2 = [0, 14.5];
p3 = [11.655, 8.741];
p4 = [0, -14.5];
p = [p2; p3; p4; p1];
px = p(:,1);
py = p(:,2);

figure;
plot(px,py,'ro'); hold on;
axis([-115, 115, -60, 60]);
lsline;
plot(mean(px),mean(py), 'kx');
plot(0,0,'k+');
grid on;

load('A.mat');
dx = rawStarData(:,1:2:8)-512;
dy = rawStarData(:,2:2:8)-512;
n = size(dx,1);


for t = [335,620,640,910,1108]
    figure;
    title(t);
    x = dx(t,:); y = dy(t,:);
    s = scatter(x,y,'ro'); hold on;
    axis([-550,550,-550,550]); grid on;
    l = lsline;
    c = plot(mean(x),mean(y),'kx');
    r = plot(0,0,'k+');
    
    pause(0.5);
%     delete(s);
%     delete(l);
%     delete(c);
%     delete(r);
end



