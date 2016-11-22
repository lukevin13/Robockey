#include <math.h>
#include <stdio.h>

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

double mean(double* data, int n) {
	double sum = 0;
	int i;
	for (i=0;i<n;i++) {
		sum += data[i];
	}
	return sum/n;
}

double mean_i(int* data, int n) {
	double sum = 0;
	int i;
	for (i=0;i<n;i++) {
		sum += data[i];
		// printf("%d\r\n", data[i]);
	}
	return sum/n;
}

int countNumStars(int* x, int* y) {
	int i;
	int numStars = 0;
	for (i=0;i<4;i++) {
		if (x[i] == 511 && y[i] == 511) numStars++;
	}
	return 4-numStars;
}

int localize_me(double* th, double* r_pos, int* x, int* y, int numStars) {
	// Setup
	int n = nchoosek(numStars);
	double distances[n];
	int point1[n];
	int point2[n];

	// If less than 3 stars return
	if (numStars < 4) return 0;

	// Calculate all distances
	int k = 0;
	int i,j;
	for (i=0;i<numStars;i++) {
		for (j=i+1;j<numStars;j++) {
			double pt1[2] = {x[i], y[i]};
			double pt2[2] = {x[j], y[j]};

			// printf("%d, %d\r\n", x[i], y[i]);
			// printf("%d, %d\r\n", x[j], y[j]);

			
			distances[k] = pdist(pt1,pt2);
			point1[k] = i;
			point2[k] = j;
			

			// printf("[%f, %d, %d]\r\n\r\n", distances[k],i,j);
			k++;
		}
	}

	// Get longest distance
	int ns_index = max_index(distances, n);
	// printf("%d\r\n", ns_index);

	// Possible north and south star
	int ns1 = point1[ns_index];
	int ns2 = point2[ns_index];
	// printf("%d - %d\r\n\r\n", ns1, ns2);

	// Calculate center of mass of points
	double cx = mean_i(x, numStars);
	double cy = mean_i(y, numStars);
	// printf("%f, %f\r\n\r\n", cx, cy);

	// Calculate distances from star to centroid
	double nspt1[2] = {x[ns1], y[ns1]};
	double nspt2[2] = {x[ns2], y[ns2]};
	double centroid[2] = {cx, cy};
	double d1 = pdist(nspt1, centroid);
	double d2 = pdist(nspt2, centroid);
	// printf("d1: %f\r\n", d1);
	// printf("d2: %f\r\n\r\n", d2);

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

	// Find distance from constellation center
	double nsx[2] = {northStar[0], southStar[0]};
	double nsy[2] = {northStar[1], southStar[1]};
	double world_c[2] = {-mean(nsx, 2), -mean(nsy, 2)};
	// printf("d: %f,%f\r\n", world_c[0], world_c[1]);

	// Find theta
	double world_y[2] = {0, 0};
	world_y[0] = northStar[0] - southStar[0];
	world_y[1] = northStar[1] - southStar[1];
	double y_th = atan2(world_y[1], world_y[0]);
	double r_th = atan2(1, 0);
	double theta = r_th - y_th;
	th[0] = theta;
	// printf("theta: %f\r\n", theta);

	// Calculate robot position
	double r_x = world_c[0]*cos(theta) - world_c[1]*sin(theta);
	double r_y = world_c[0]*sin(theta) + world_c[1]*cos(theta);
	// printf("r: %f, %f\r\n", r_x, r_y);

	// Find scale
	double scale = 14.5/pdist(northStar, southStar)/2;
	scale = 1;
	// printf("s: %f\r\n", scale);

	// Apply scale
	r_x *= scale;
	r_y *= scale*(768.0/1024);

	// Return position in the rink
	r_pos[0] = r_x;
	r_pos[1] = r_y;
	// printf("r_pos: %f, %f\r\n", r_pos[0], r_pos[1]);
	return 1;
}
