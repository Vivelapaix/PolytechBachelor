#include<stdio.h>
#include<stdlib.h>

#define epsilon 0.0000001

double f(double y, double y0, double x, double h)
{
  return /*y - y0 - h*(2*x - 3*y)*/   //Точное  y = -2/9 + 2/3*x + 11/9*exp(-3*x);
	     /*y - y0 - h*(5*y - x*x)*/   //Точное y = x*x + 0.4*x + 0.08;
		 y - y0 - h*(-100*y + 10)
	  ;
}

double df(double y1, double y0, double x, double h)
{
   return /*1 + 3*h*/
	       /*1 - 5*h*/
		   1 + 100*h
	   ;
}

double NewtonMethod(double yInit, double yPrev, double x, double h)
{
  double y = 0, y0 = yInit;
  double valF = 0, valdF = 0;
  while(1)
  {
    valF = f(y0, yPrev, x, h);
	valdF = df(y0, yPrev, x, h);
    y = y0 - valF/valdF;
    y0 = y;  
    if(f(y + epsilon, yPrev, x, h) > 0 && f(y - epsilon, yPrev, x, h) < 0)
    {
      break;
    }
  }
  return y;
}


void ImplicitEulerMethod(char *FileName, double a, double b, int n, double X[], double Y[])
{
  FILE *file = NULL;
  double h = (b - a)/(double)n;  

  fopen_s(&file, FileName, "w");

  fprintf(file,"%.15lf %.15lf\n", X[0], Y[0]);
  for(int i = 1; i < n + 1; i++)
  {
	X[i] = X[0] + i*h;
    Y[i] = NewtonMethod(Y[i - 1], Y[i - 1], X[i], h);
	fprintf(file,"%.15lf %.15lf\n", X[i], Y[i]);
  }
  fclose(file);
}


int main(void)
{
  int n = 50;
  double *X = NULL, *Y = NULL;
  char *FileName = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\Impl.txt";

  X = (double*)malloc((n + 1)*sizeof(double));
  Y = (double*)malloc((n + 1)*sizeof(double));

  /*X[0] = 0;
  Y[0] = 1;*/
  /*X[0] = 0;
  Y[0] = 0.08;*/
  X[0] = 0;
  Y[0] = 1.0;
  ImplicitEulerMethod(FileName, 0, 1, n, X, Y);

  free(X);
  free(Y);

  return 0;
}