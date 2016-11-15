close all;
clear all;

% Formulas from: 
% https://www.pozyx.io/Documentation/doc_howDoesPositioningWork

% Load data
load('A.mat');

% Preprocess
data = rawStarData;
n = size(data,1);
for i = 1:2:8
    check1 = data(:,i) == 1023;
    check2 = data(:,i+1) == 1023;
    check = check1 | check2;
    data(check,i) = NaN;
    data(check,i+1) = NaN;
end

data = data - 512;

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

% Extract x and y of seen stars
z = zeros(n,1);
ssx = logical([s1g,z,s2g,z,s3g,z,s4g,z]);
ssy = logical([z,s1g,z,s2g,z,s3g,z,s4g]);
dg = [s1g,s2g,s3g,s4g];
numStars = sum(ssx,2);

% Calculate squared distance from robot to stars
s1d = sum(star1.^2,2);
s2d = sum(star2.^2,2);
s3d = sum(star3.^2,2);
s4d = sum(star4.^2,2);
dd = [s1d,s2d,s3d,s4d];

% Calculate robot position
robot = zeros(n,2); % [x,y]
for i = 1:n
    if (numStars(i) < 3)
        if (i == 1)
            robot(i,1) = NaN;
            robot(i,2) = NaN;
        else
            robot(i,:) = robot(i-1,:);
        end
        continue;
    end

    sx = data(i,ssx(i,:))';
    sy = data(i,ssy(i,:))';
    di = dd(i,dg(i,:))';
    
    x = sx(1:end-1); xn = sx(end);
    y = sy(1:end-1); yn = sy(end);
    d = di(1:end-1); dn = di(end);
    
    A = -2*[x-xn y-yn];
    b = d - x.^2 - y.^2 - dn + xn^2 + yn^2;
    
    if (numStars(i) == 3)
        robot(i,:) = (b\A)';
    else
        robot(i,:) = (pinv(A)*b)';
    end
end

% Plot robot path
plot(robot(:,1), robot(:,2), 'r-');
    
    