close all;
clear all;

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

% Calculate Robot Position
robot_o = [512, 384];
robot = zeros(n,1);
for i = 1:n
    N = numStars(i);
    robot(i,1) = NaN;
    robot(i,2) = NaN;
    if (N >= 3)
        x = data(i,ssx(i,:))';
        y = data(i,ssy(i,:))';

        sky_o = [mean(x), mean(y)];
        v = robot_o - sky_o;
        th = atan2(v(2),v(1));
        robot(i,:) = [cos(th), sin(th)];
    end
end

% Calculate movement vectors
move = robot - [robot(1,:);robot(1:end-1,:)];
for i = 1:n
    move(i,:) = 0.25*move(i,:)/norm(move(i,:));
end

% Plot robot path
figure(1);
title('Robot Path');
% axis([0 1024 0 768]);
plot(robot(:,1), robot(:,2));

% % Animate robot
% figure(2);
% title('Robot Path Animated');
% hold on;
% axis([-2 2 -2 2]);
% for i = 2:n
%     r = plot(robot(i,1), robot(i,2),'k+');
%     q = quiver(robot(i,1), robot(i,2), move(i,1), move(i,2),...
%         'AutoScale','off','MaxHeadSize',0.8,'Color','k');
%     pause(0.002);
%     delete(r);
%     delete(q);
% end