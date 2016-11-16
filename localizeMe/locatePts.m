function [sr,er,nr,wr] = locatePts(x, y)

% World frame distances
w = [-10.563, 2.483];
n = [0, 14.5];
e = [11.655, 8.741];
s = [0, -14.5];
c = [0.2730 2.8060];

cw = sqrt(sum(c-w).^2);
cn = sqrt(sum(c-n).^2);
ce = sqrt(sum(c-e).^2);
cs = sqrt(sum(c-s).^2);
d_w = sort([cw,cn,ce,cs],'descend');

pt1 = [x(1), y(1)];
pt2 = [x(2), y(2)];
pt3 = [x(3), y(3)];
pt4 = [x(4), y(4)];
cen = [mean(x), mean(y)];

pts = [x;y]';
d12 = [sqrt(sum(pt1-pt2).^2) 1 2];
d13 = [sqrt(sum(pt1-pt3).^2) 1 3];
d14 = [sqrt(sum(pt1-pt4).^2) 1 4];
d23 = [sqrt(sum(pt2-pt3).^2) 2 3];
d24 = [sqrt(sum(pt2-pt4).^2) 2 4];
d34 = [sqrt(sum(pt3-pt4).^2) 3 4];

dc = [d12',d13',d14',d23',d24',d34'];
[d_c, I_c] = sort(dc(1,:),'descend');

ns1 = dc(2,I_c(1));
ns2 = dc(3,I_c(1));
p1 = pts(ns1,:);
p2 = pts(ns2,:);
d1 = sqrt(sum(p1-cen).^2);
d2 = sqrt(sum(p2-cen).^2);

% if (d1 > d2)
%     sr = ns1;
%     nr = ns2;
% else
%     sr = ns2;
%     nr = ns1;
% end

% a = [1,2,3,4];
% b = (a == nr | a == sr);
% c = a(~b);
% er = c(1);
% wr = c(2);
nr = 2;
sr = 4;
er = 1;
wr = 3;
end