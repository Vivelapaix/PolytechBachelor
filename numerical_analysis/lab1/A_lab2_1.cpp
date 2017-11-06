#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define a 0.0
#define b 3.1415926535
#define pi 3.1415926535

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


double gridCheb(int i, int n)
{
  return ((a + b) / 2 + (b - a) / 2 * cos((2 * i + 1) * pi / (2 * (n + 1))));
}



int main(void)
{
  FILE *file = NULL;
  int j = 0, k = 0, n = 100;
  double i = 0, count = 0, err = 0, tmp = 0;
  double *X, *Y;

  fopen_s(&file, "out.txt", "w");

  for(j = 2; j < n; j++)
  {
    X = (double*)malloc((j + 1)*sizeof(double));
    Y = (double*)malloc((j + 1)*sizeof(double));

	/*Равномерная сетка*/
	count = b/j;
    k = 0;
    for(i = a; i <= b; i += count)
    {
      X[k] = i;
	  Y[k] = sin(i);
      k++;
    }

    /*Сетка  из корней многочлена Чебышёва*/
    /*for(k = 0; k <= j; k++)
    {
      X[k] = gridCheb(k, j);
	  Y[k] = sin(X[k]);
    }*/

    for(i = a; i <= b; i += 0.001)
    {
      tmp = fabs(PolyLagrange(i, X, Y, k) - sin(i));
      if(err < tmp)
      {
        err = tmp;
      }
    }

    /*for(i = 0; i <= 3.1417; i += 0.01)
    {
      fprintf(file, "%.15lf %.15lf\n", i, PolyLagrange(i, X, Y, k));
    }*/
    
    fprintf(file, "%.15lf %i\n", err, j);
    err = 0;

    free(X);
    free(Y);
  }

  fclose(file);
 return 0;
}