close all;
clear all;

w = [-10.563, 2.483];
n = [0, 14.5];
e = [11.655, 8.741];
s = [0, -14.5];
p = [n; e; s; w];
px = p(:,1);
py = p(:,2);

mpx = mean(px);
mpy = mean(py);
centroid = [mpx, mpy];



