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

% Plot stars
figure(1);
for t = 1:ndata
    if (numStars(t) > 3)
        mask = ~data_check(t,:);
        x = x_data(t,mask);
        y = y_data(t,mask);
        p1 = plot(x(2),y(2),'ro'); hold on;
        p2 = plot(x(4),y(4),'ro');
        p3 = plot(x(1),y(1),'ko');
        p4 = plot(x(3),y(3),'ko');
        
        axis([-550 550 -550 550]);
        pause(0.01);
        
        delete(p1);
        delete(p2);
        delete(p3);
        delete(p4);
    end
end