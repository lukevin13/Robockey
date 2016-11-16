close all;
clear all;

% Load data
load('A.mat');
data = rawStarData;
x_data = data(:,1:4);
y_data = data(:,5:8);

% Check for missing stars
x_check = x_data == 1023;
y_check = y_data == 1023;
data_check = x_check & y_check;
numStars = 4-sum(data_check,2);
ndata = size(data,1);
x_data = x_data - 512;
y_data = y_data - 512;

% Calculate position
r_pos = NaN*zeros(ndata,2);
for t = 1:ndata
    if (numStars(t) >= 3)
        % Find north and south stars
        mask = ~data_check(t,:);
        x = x_data(t,mask)';
        y = y_data(t,mask)';
        [ind_n, ind_s] = find_y(x,y);
        
        % Caluclate signed angle between robot y_axis and world y-axis
        northStar = [x(ind_n) y(ind_n)];
        southStar = [x(ind_s) y(ind_s)];
        world_y = northStar - southStar;
        world_theta = atan2(world_y(2), world_y(1));
        robot_theta = atan2(1, 0);
        theta = robot_theta - world_theta;
        
        % Create Transformation Matrices
        R = [cos(theta) -sin(theta); sin(theta) cos(theta)];
        T = -[mean([x(ind_n) x(ind_s)]); mean([y(ind_n) y(ind_s)])];
        S = [1; 14.5/(world_y/2)];
        
        % Calculate robot position
        r_pos(t,:) = ((R*T).*S)';
    end 
end

% Plot path
figure(1); title('Robot Path');
plot(r_pos(:,1),r_pos(:,2),'r-');
% axis([-550 550 -550 550]);

% % Plotting
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