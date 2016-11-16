% descs1 is a 64x(n1) matrix of double values
% descs2 is a 64x(n2) matrix of double values
% match is n1x1 vector of integers where m(i) points to the index of the
% descriptor in p2 that matches with the descriptor p1(:,i).
% If no match is found, m(i) = -1

function [match] = feat_match(descs1, descs2)
n = size(descs1,2);
threshold = 0.6;
match = zeros(n,1);

% Build Kd-tree from image 2
d2 = KDTreeSearcher(descs2');

% Compare pairwise points
for i = 1:n
    y = descs1(:,i);
    
    % Search for two nearest neighbors
    nnbrs = knnsearch(d2, y', 'K', 2, 'IncludeTies', true);
    idx1 = nnbrs{1}(1);
    idx2 = nnbrs{1}(2);
    
    % Get values of the nearest neighbors from descs2
    n1 = descs2(:,idx1);
    n2 = descs2(:,idx2);
    
    % Calculate SSD
    ssd1 = sum((y-n1).^2);
    ssd2 = sum((y-n2).^2);
    
    % Check if good match
    if (ssd1/ssd2 < threshold)
        match(i) = idx1;
    else
        match(i) = -1;
    end
end
end