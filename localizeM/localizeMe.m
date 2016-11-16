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
y_data = 512 - y_data;

% Calculate robot position
v = NaN*zeros(ndata,2);
th = NaN*zeros(ndata,1);
for t = 1:ndata
    if (numStars(t) > 3)
        mask = ~data_check(t,:);
        x = x_data(t,mask);
        y = y_data(t,mask);
        
        npt = [x(2),y(2)];
        spt = [x(4),y(4)];
        world_y = spt - npt;
        costh = dot([0 1], world_y)/(norm(world_y));
        theta = acosd(costh);
        th(t) = theta;
        world_c = [mean([npt(1),spt(1)]), mean([npt(2),spt(2)])]';
        A = [cosd(theta) -sind(theta); sind(theta) cosd(theta)];
        v(t,:) = (A*world_c)';
    end
end

% % Plot path
% figure(1);
% 
% for t = 2:ndata
%     r = plot(v(t,1), v(t,2), 'ro'); hold on;
%     q = quiver(v(t,1),v(t,2),v(t,1)-v(t-1,1),v(t,2)-v(t-1,2));
%     axis([-550 550 -550 550]);
%     pause(0.01);
%     delete(r);
%     delete(q);
% end


% Plot stars
figure(2);
for t = 1:ndata
    if (numStars(t) > 3)
        title(th(t));
        mask = ~data_check(t,:);
        x = x_data(t,mask);
        y = y_data(t,mask);
        
        p1 = plot(x(2),y(2),'ro'); hold on;     
        p2 = plot(x(4),y(4),'ro');
        p3 = plot(x(1),y(1),'ko');
        p4 = plot(x(3),y(3),'ko');
        
        world_c = [mean([x(2),x(4)]), mean([y(2),y(4)])]';
        l1 = line([x(2),x(4)],[y(2),y(4)]);
        l2 = line([world_c(1), 0],[world_c(2), 0]);

        axis([-550 550 -550 550]);
        pause(0.01);
        
        delete(p1);
        delete(p2);
        delete(p3);
        delete(p4);
        delete(l1);
        delete(l2);
    end
end