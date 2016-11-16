close all;
clear all;

% Load data
datafile = 'C.mat';
load(datafile);
data = rawStarData;
x_data = data(:,1:4);
y_data = data(:,5:8);

% Check for missing stars
x_check = x_data == 1023;
y_check = y_data == 1023;
data_check = x_check & y_check;
numStars = 4-sum(data_check,2);
ndata = size(data,1);
x_data = 512 - x_data;
y_data = 512 - y_data;

% Calculate position
r_pos = NaN*zeros(ndata,2);
r_dir = NaN*zeros(ndata,2);
for t = 1:ndata
    if (numStars(t) >= 3)
        % Find north and south stars
        mask = ~data_check(t,:);
        x = x_data(t,mask)';
        y = y_data(t,mask)';
        [ind_n, ind_s] = find_y(x,y);
        
        % Calculate signed angle between robot axis and world axis
        northStar = [x(ind_n) y(ind_n)];
        southStar = [x(ind_s) y(ind_s)];
        world_y = northStar - southStar;
        world_theta = atan2(world_y(2), world_y(1));
        robot_theta = atan2(1, 0);
        theta = robot_theta - world_theta;
        
        % Create Transformation Matrices
        T = -[mean([x(ind_n) x(ind_s)]); mean([y(ind_n) y(ind_s)])];
        R = [cos(theta) -sin(theta); sin(theta) cos(theta)];
        S = 14.5/norm(world_y/2)*ones(2,1);
        
        % Calculate robot position
        r_pos(t,:) = ((R*T).*S)';
        r_dir(t,:) = (R*[1;0])';
    end 
end

% Calculate direction
if (~strcmp(datafile,'A.mat'));
    r_dir = r_pos - [r_pos(1,:);r_pos(1:end-1,:)];
end;
% r_dir = 5*r_dir ./ repmat(sqrt(sum(r_dir.^2,2)),1,2);

% Plot path
res = 1:2:ndata;
figure(1); title('Robot Path - C.mat'); hold on; grid on;
% plot(r_pos(:,1),r_pos(:,2),'r-');
quiver(r_pos(res,1),r_pos(res,2),r_dir(res,1),r_dir(res,2),5);
axis([-150 150 -80 80]);

% % Plotting Stars
% figure;
% for t = 1:ndata
%     if (numStars(t) >= 3)
%         mask = ~data_check(t,:);
%         x = x_data(t,mask)';
%         y = y_data(t,mask)';
%         [ind_n, ind_s] = find_y(x,y);
%         a = 1:numStars(t);
%         b = a == ind_n | a == ind_s;
%         stars = a(~b);
%         
%         p1 = plot(x(ind_n), y(ind_n), 'ro'); hold on;
%         p2 = plot(x(ind_s), y(ind_s), 'bo');
%         s = scatter(x(stars), y(stars), 'ko');
%         
%         axis([-550 550 -550 550]);
%         pause(0.01);
%         
%         delete(p1);
%         delete(p2);
%         delete(s);
%     end
% end