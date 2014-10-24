#include<iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include"stdlib.h"


#define SMALL 10E-12
#define Pi    3.14159265358979323846
#define LEAK        5
#define INJECTOR    4
#define PRODUCER    3
#define BORDER      2
#define OPEN        1
#define NORMAL      0

#define ALPHA_MAX   1.0
#define ALPHA_MIN   0.0


double amMIN(double a, double b);
double amMAX(double a, double b);
void multiply(int Nt, int N, int** Coort, double*** A, double* b, double* Ab);
