function [n, s] = find_y(x,y)
    ndata = size(x,1);
    
    pts = [x,y];
    d_c = zeros(nchoosek(ndata,2),3);
    k = 1;
    for i = 1:ndata
        for j = i+1:ndata
            d_c(k,1) = pdist([pts(i,:);pts(j,:)],'euclidean');
            d_c(k,2) = i;
            d_c(k,3) = j;
            k = k + 1;
        end
    end
    
    [~, i_c] = sort(d_c(:,1),'descend');
    
    ns1 = d_c(i_c(1), 2);
    ns2 = d_c(i_c(1), 3);
    
    c = [mean(x) mean(y)];
    d1 = pdist([c;pts(ns1,:)],'euclidean');
    d2 = pdist([c;pts(ns2,:)],'euclidean');
    
    if (d1 < d2)
        n = ns1;
        s = ns2;
    else
        n = ns2;
        s = ns1;
    end
    
end