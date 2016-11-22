#include <math.h>
#include <stdio.h>

double pdist(double* pt1, double*pt2);
int nchoosek(int numStars);
int max_index(double* distances, int n);
double mean(double* data, int n);
double mean_i(int* data, int n);
int countNumStars(int* x, int* y);
int localize_me(double* th, double* r_pos, int* x, int* y, int numStars);