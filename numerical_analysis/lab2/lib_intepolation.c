#include "interpolation.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void UniformGrid(const double a, const double b, const int n, double *X, double *Y)
{
  double step = (b - a)/(n - 1);
  int i = 0;
  for(i = 0; i < n; i++)
  {
    X[i] = a + i * step;
	Y[i] = sin(X[i]);
  }
}


double gridCheb(const double a, const double b, const double pi, int i, int n)
{
  return ((a + b) / 2 + (b - a) / 2 * cos((2 * i + 1) * pi / (2 * (n + 1))));
}

double PolyLagrange(double x, double *X, double *Y, int n)
{
  int i = 0, j = 0;
  double y = 0, lj = 1;

  for(i = 0; i < n; i++)
  {
    lj = 1;
    for(j = 0; j < n; j++)
	{
      if(j != i)
	  {
        lj *= (x - X[j]) / (X[i] - X[j]);
	  }
	}
    y += Y[i] * lj;
  }
  return y;
}

