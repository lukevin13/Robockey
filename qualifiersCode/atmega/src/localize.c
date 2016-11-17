#include <math.h>

double pdist(double* pt1, double*pt2) {
	double dx = pt1[0] - pt2[0];
	double dy = pt1[1] - pt2[1];
	double d2 = dx*dx + dy*dy;
	return sqrt(d2);
}

int nchoosek(int numStars) {
	if (numStars == 3) return 3;
	if (numStars == 4) return 6;
	return 1;
}

int max_index(double* distances, int n) {
	double max_dist = -1;
	int max_index = -1;
	int i;
	for (i=0;i<n;i++) {
		if (distances[i] > max_dist) {
			max_dist = distances[i];
			max_index = i;
		}
	}
	return max_index;
}

double mean(int* data, int n) {
	double sum = 0;
	int i;
	for (i=0;i<n;i++) {
		sum += data[i];
	}
	return sum/n;
}

double* localize_me(int* x, int* y, int numStars) {
	// Setup
	int n = nchoosek(numStars);
	double distances[n];
	int point1[n];
	int point2[n];

	// Calculate all distances
	int k = 0;
	int i,j;
	for (i=0;i<numStars;i++) {
		for (j=i+1;j<numStars;j++) {
			double pt1[2] = {x[i], y[i]};
			double pt2[2] = {x[j], y[j]};
			
			distances[k] = pdist(pt1,pt2);
			point1[k] = i;
			point2[k] = j;
			k++;
		}
	}

	// Get longest distance
	int ns_index = max_index(distances, n);

	// Possible north and south star
	int ns1 = point1[ns_index];
	int ns2 = point2[ns_index];

	// Calculate center of mass of points
	double cx = mean(x, n);
	double cy = mean(y, n);

	// Calculate distances from star to centroid
	double nspt1[2] = {x[ns1], y[ns1]};
	double nspt2[2] = {x[ns2], y[ns2]};
	double centroid[2] = {cx, cy};
	double d1 = pdist(nspt1, centroid);
	double d2 = pdist(nspt2, centroid);

	// Assign north and south stars
	double* northStar;
	double* southStar;
	if (d1 < d2) {
		northStar = nspt1;
		southStar = nspt2;
	} else {
		northStar = nspt2;
		southStar = nspt1;
	}

	double r_pos[2] = {0,0};
	return r_pos;
}
