#include <math.h>
#include <stdio.h>

double pdist(double* pt1, double*pt2);
int nchoosek(int numStars);
int max_index(double* distances, int n);
double mean(double* data, int n);
double mean_i(unsigned int* data, int n);
int countNumStars(unsigned int* x, unsigned int* y);
int localize_me(double* r_pos, unsigned int* x, unsigned int* y);