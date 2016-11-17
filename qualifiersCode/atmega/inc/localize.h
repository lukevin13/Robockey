/* Find the north and south stars - Kevin Lu */

#include <math.h>

// Calculate 2D euclidean distance between points
double pdist(double* pt1, double* pt2);

// Get robot (x,y) position in the rink
double* localize_me(int* x, int*y, int numStars);
