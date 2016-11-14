% take in variables
m = matfile('A.mat');
rawStarData = m.rawStarData;

%% instantiate
l = [29;26;23.08251;20;16;13];

theta = zeros(1,length(rawStarData));
xr_r = zeros(2,length(rawStarData));

for t = 1:length(rawStarData(:,1))
    
    x = [rawStarData(t,1) rawStarData(t,3) rawStarData(t,5) rawStarData(t,7);
        rawStarData(t,2) rawStarData(t,4) rawStarData(t,6) rawStarData(t,8)];

    %% obtain relative distances
    d = [0 1 2;
        0 1 3;
        0 1 4;
        0 2 3;
        0 2 4;
        0 3 4];
    k=1;
    for i = 1:4
        for j = i+1:4
            if i == j
                continue;
            end
            d(k,1) = sqrt((x(1,i) - x(1,j))^2 + (x(2,i)-x(2,j))^2);
            k = k+1;
        end
    end

    %% find order
    o = [1;2;3;4;5;6];
    for i = 2:length(o)
        j = i;
        while j > 1 && d(o(j-1),1) < d(o(j),1)
            temp = o(j);
            o(j) = o(j-1);
            o(j-1) = temp;
            j = j-1;
        end
    end

    %% determine which point is which
    r2c = zeros(4,1);
    p1 = d(o(1), 2);
    p2 = d(o(1), 3);
    p3 = d(o(2), 2);
    p4 = d(o(2), 3);
    if p1 == p3
        r2c(1) = p1;
        r2c(2) = p2;
        r2c(3) = p4;
    elseif p1 == p4
        r2c(1) = p1;
        r2c(2) = p2;
        r2c(3) = p3;
    elseif p2 == p3
        r2c(1) = p2;
        r2c(2) = p1;
        r2c(3) = p4;
    elseif p2 == p4
        r2c(1) = p2;
        r2c(2) = p1;
        r2c(3) = p3;
    else
        disp('error');
        
        
        continue;
    end

    for i = 1:4
        if r2c(1) ~= i && r2c(2) ~= i && r2c(3) ~= i
            r2c(4) = i;
        end
    end

    %% calculate theta and del_x

    v = [x(1, r2c(2))-x(1, r2c(1)); x(2,r2c(2))-x(2,r2c(1))];
    scale = abs(l(1) / norm(v));
    
    if v(1) > 0
        theta(t) = acos(v(2)/norm(v));
    else
        theta(t) = acos(v(2)/norm(v)) - pi;
    end

    p1_origin = x(:,r2c(1)) - [512;512];
    xc_c = p1_origin*scale + l(1)/2*[-sin(theta(t)); cos(theta(t))];

    xr_r(:, t) = -xc_c;
end

figure(1);
hold;
quiver(sin(theta), cos(theta), xr_r(1,:), xr_r(2,:));
