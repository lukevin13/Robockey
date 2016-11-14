close all;
clear all;

% Formulas from: 
% https://www.pozyx.io/Documentation/doc_howDoesPositioningWork

% Load data
load('A.mat');

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
data = data - 512;
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
    
    N = numStars(i);
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

% Calculate movement vectors
move = robot - [robot(1,:);robot(1:end-1,:)];
for i = 1:n
    move(i,:) = 70*move(i,:)/norm(move(i,:));
end

% Animate sky
% figure;
% hold on;
% axis([0 1024 0 768]);
% plot(cx,cy,'k+');
% for i = 1:n   
%     Plot the stars
%     s1 = plot(star1(i,1),star1(i,2),'ro');
%     s2 = plot(star2(i,1),star2(i,2),'bo');
%     s3 = plot(star3(i,1),star3(i,2),'go');
%     s4 = plot(star4(i,1),star4(i,2),'co');
%     
%     di = dd(i,dg(i,:))';
%     
%     Plot circles
%     c1 = viscircles(star1(i,:),
%     
%     Animation delay
%     pause(0.003);
%     
%     Clear the stars
%     delete(s1);
%     delete(s2);
%     delete(s3);
%     delete(s4);
%     
% end

% Plot robot path
figure(1);
title('Robot Path');
axis([0 1024 0 768]);
plot(robot(:,1), robot(:,2));

% % Animate robot
% figure(2);
% title('Robot Path Animated');
% hold on;
% axis([0 1024 0 768]);
% for i = 2:n
%     figure(2);
%     r = plot(robot(i,1), robot(i,2),'k+');
%     q = quiver(robot(i,1), robot(i,2), move(i,1), move(i,2),...
%         'AutoScale','off','MaxHeadSize',0.8,'Color','k');
%     pause(0.003);
%     delete(r);
%     delete(q);
% end
    
    