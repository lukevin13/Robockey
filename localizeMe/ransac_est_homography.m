% y1, x1, y2, x2 are the corresponding point coordinate vectors Nx1 such
% that (y1i, x1i) matches (x2i, y2i) after a preliminary matching

% thresh is the threshold on distance used to determine if transformed
% points agree

% H is the 3x3 matrix computed in the final step of RANSAC

% inlier_ind is the nx1 vector with indices of points in the arrays x1, y1,
% x2, y2 that were found to be inliers

function [H, inlier_ind] = ransac_est_homography(x1, y1, x2, y2, k, thresh)
n = length(x1); % n points
max_inliers = 0;

% Loop while running RANSAC
for i = 1:200
    % Select 4 random points
    r_pts = randperm(n,k);
    
    % est_homography the points
    h = est_homography(x2(r_pts), y2(r_pts), x1(r_pts), y1(r_pts));
    
    % Compute inliers
    [xp, yp] = apply_homography(h, x1, y1);
    ssdh = sum(([(xp-x2),(yp-y2)].^2),2);
    inliers_bool = ssdh < thresh;
    
    % If new max, save the inliers
    num_inliers = sum(inliers_bool);
    if (num_inliers > max_inliers)
        max_inliers = num_inliers;
        ind1 = find(inliers_bool);
    end

end

% Return recomputed H
H = est_homography(x1(ind1), y1(ind1), x2(ind1), y2(ind1));
inlier_ind = ind1;
end