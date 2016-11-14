close all;
clear all;

load('C.mat');

% Preprocess
data = rawStarData;
n = size(data,1);
cx = 1024/2;
cy = 768/2;
for i = 1:2:8
    check1 = data(:,i) == 1023;
    check2 = data(:,i+1) == 1023;
    check = check1 | check2;
    data(check,i) = NaN;
    data(check,i+1) = NaN;
end

% Scale
data(:,[2,4,6,8]) = data(:,[2,4,6,8]) * (768/1023);

% Assign the data
star1 = data(:,1:2);
star2 = data(:,3:4);
star3 = data(:,5:6);
star4 = data(:,7:8);

% Determine for unseen stars
s1g = ~isnan(star1(:,1));
s2g = ~isnan(star2(:,1));
s3g = ~isnan(star3(:,1));
s4g = ~isnan(star4(:,1));

% Calcuations
ref = [0, 1];

% Extract x and y of seen stars
z = zeros(n,1);
ssx = logical([s1g,z,s2g,z,s3g,z,s4g,z]);
ssy = logical([z,s1g,z,s2g,z,s3g,z,s4g]);
dg = [s1g,s2g,s3g,s4g];
numStars = sum(ssx,2);

% Calculate Robot Position
robot_o = [512, 512];
robot = zeros(n,1);
for i = 1:n
    N = numStars(i);
    robot(i,1) = NaN;
    robot(i,2) = NaN;
    if (N >= 3)
        x = data(i,ssx(i,:))';
        y = data(i,ssy(i,:))';
        
        pts = [x, y]';
        th1 = acosd(dot(pts(:,1),ref')/(norm(pts(:,1))*norm(ref')));
        th2 = acosd(dot(pts(:,2),ref')/(norm(pts(:,2))*norm(ref')));
        T12 = cotd(th2-th1);
        
        x1 = x(1) - x(2);
        x3 = x(3) - x(2);
        y1 = y(1) - y(2);
        y3 = y(3) - y(2);
        
        x12 = x1 + T12*y1;
        x23 = x3 + T12*y3;
        x31 = y3-y1;
        y12 = y1 - T12*x1;
        y23 = y3 - T12*x3;
        y31 = x1 - x3;
        
        k31 = x1*y3 - x3*y1;
        D = (x23 - x12)*y31 + (y12 - y23)*(x31);
        xr = x(2) + (k31*(y12-y23))/D;
        yr = y(2) + (k31*(x23-x12))/D;

        robot(i,1) = xr;
        robot(i,2) = yr;
    end
end

% Plot robot path
figure(1);
title('Robot Path');
axis([0 1024 0 768]);
plot(robot(:,1), robot(:,2));