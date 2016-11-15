close all;
clear all;
load('A.mat');

% Preprocess
data = rawStarData;
n = size(data,1);

% Check for absence of star
for i = 1:2:8
    check_x = data(:,i) == 1023;
    check_y = data(:,i+1) == 1023;
    check_p = check_x | check_y;
    data(check_p,i) = NaN;
    data(check_p,i+1) = NaN;
end

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
numStars = sum(ssx,2);
starIndex = find(numStars >= 3);

starData = data(starIndex,:);
sx = ssx(starIndex,:);
sy = ssy(starIndex,:);
nStars = numStars(starIndex);

v = zeros(n,2);

