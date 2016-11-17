#include "localize.c"
#include <stdio.h>

int main() {

	// double p1[2] = {10.0, 0};
	// double p2[2] = {5.0, 0};

	// // Test pdist
	// double d = pdist(p1, p2);
	// printf("d: %lf\r\n", d);

	// // Test mean
	// double data[4] = {1,2,3,4};
	// double m = mean(data, 4);
	// printf("m: %lf\r\n", m);

	// Test data - {n, e, w, s}
	int x[4] = {10, 12, 9, 10};
	int y[4] = {4, 2, 3, -4};
	double pos[2] = {0,0};
	localize_me(pos,x,y,4);
	printf("x: %f\r\n", pos[0]);
	printf("y: %f\r\n", pos[1]);
	return 1;
}