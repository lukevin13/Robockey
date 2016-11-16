
% Load data
load('C.mat');
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
theta = zeros(n,1);
senw = zeros(n,4);

% Calculate
for t = 1:n 
    if (numStars(t) > 3)
        sx = x(t,:);
        sy = y(t,:);
%         px = sx(~check(t,:));
%         py = sy(~check(t,:));
        [sc,ec,nc,wc] = locatePts(sx,sy);
        senw(t,:) = [sc,ec,nc,wc];
        north = [sx(nc), sy(nc)];
        south = [sx(sc), sy(sc)];
        world_y = north - south;
        theta(t) = acos(dot([0,1],world_y)/(norm(world_y)));
        R = [cos(theta(t)) -sin(theta(t)); sin(theta(t)) cos(theta(t))];
        cen = [mean(sx),mean(sy)];
        dt = R*(-cen');
        v(t,:) = dt;
    else
        v(t,:) = NaN;
        theta(t) = NaN;
    end
end

% Plot
figure;
plot(v(:,1),v(:,2),'r-');
axis([-550,550,-550,550]);

figure;
for t = 1:n
    if (numStars(t) > 3)
        st = senw(t,1);
        et = senw(t,2);
        nt = senw(t,3);
        wt = senw(t,4);
        
        ps = plot(x(t,st),y(t,st),'bo'); hold on;
        pn = plot(x(t,nt),y(t,nt),'ro');
        pe = plot(x(t,et),y(t,et),'ko');
        pw = plot(x(t,wt),y(t,wt),'ko');
        
        l = line([x(t,st) x(t,nt)], [y(t,st) y(t,nt)]);
        
        axis([-550,550,-550,550]);
        pause(0.01);
        
        delete(ps);
        delete(pn);
        delete(pe);
        delete(pw);
        delete(l);
    end
end