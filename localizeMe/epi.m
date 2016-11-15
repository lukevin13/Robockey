
% Load data
load('A.mat');
data = rawStarData;
x = data(:,1:4);
y = data(:,5:8);
n = size(data,1);

% Check for missing stars
checkx = x == 1023;
checky = y == 1023;
check = logical(checkx & checky);
numStars = 4-sum(check,2);

% Preassignment
x = x - 512;
y = y - 512;
v = zeros(n,2);

% Calculate
for t = 1:n 
    if (numStars(t) >= 3)
        sx = x(t,:);
        sy = y(t,:);
        px = sx(~check(t,:));
        py = sy(~check(t,:));
        cen = [mean(px) mean(py)];
        v(t,:) = -cen;
    else
        v(t,:) = NaN;
    end
end

% Plot
figure;
plot(v(:,1),v(:,2),'r-');
axis([-550,550,-550,550]);

% figure;
% plot(0,0,'k+'); hold on;
% for t = 1:n
%     if (numStars(t) > 3)
%         s = scatter(x(t,:),y(t,:),'ro');
%         c = plot(mean(x(t,:)),mean(y(t,:)),'kx');
%         axis([-550,550,-550,550]);
%         pause(0.01);
%         delete(s);
%         delete(c);
%     end
% end