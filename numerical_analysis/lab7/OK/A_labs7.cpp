#include<stdio.h>
#include<stdlib.h>
#include<math.h>


void solveMatrix (int n, double *a, double *b, double *c, double *f, double *x)
{
	double m;
	for (int i = 1; i < n; i++)
	{
	  m = a[i]/b[i-1];
	  b[i] = b[i] - m*c[i-1];
	  f[i] = f[i] - m*f[i-1];
	}
 
	x[n-1] = f[n-1]/b[n-1];
 
	for (int i = n - 2; i >= 0; i--)
	{
	  x[i]=(f[i]-c[i]*x[i+1])/b[i];
	}
    /*double *A = NULL, *B = NULL;
    double tmp = 0;

	A = (double*)calloc(n, sizeof(double));
    B = (double*)calloc(n, sizeof(double));

    A[0] = -c[0] / b[0];
	B[0] = d[0] / b[0];

	for(int i = 1; i < n - 1; i++)
	{
	  tmp = a[i]*A[i - 1] + b[i];
	  A[i] = -c[i] / tmp;
	  B[i] = (d[i] - a[i]*B[i - 1]) / tmp;
	}

	x[n - 1] = (d[n - 1] - a[n - 1]*B[n - 2]) / (b[n - 1] + a[n - 1]*A[n - 2]);

	for(int i = n - 2; i >= 0; i--)
	{
		x[i] = A[i]*x[i + 1] + B[i];
	}*/
}


double p(double x)
{
  return 0
	  ;
}

double q(double x)
{
  return 0
	  ;
}

double f(double x)
{
  return sin(x)
	  ;
}

void FiniteDifference(const char *FileName, int n, double xinit, double xfinish, double a0, double a1, double b0, double b1, double A, double B)
{
  FILE *file = NULL, *file1 = NULL;
  double h = (xfinish - xinit) / n, max = 0;
  double *a = NULL, *b = NULL, *c = NULL, *d = NULL, *x = NULL, *y = NULL;

  fopen_s(&file, FileName, "w");
  fopen_s(&file1, "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\err.txt", "a+");

  a = (double*)malloc((n+1)*sizeof(double));
  b = (double*)malloc((n+1)*sizeof(double));
  c = (double*)malloc((n+1)*sizeof(double));
  x = (double*)malloc((n+1)*sizeof(double));
  y = (double*)malloc((n+1)*sizeof(double));
  d = (double*)malloc((n+1)*sizeof(double));

  a[0] = 0; c[n] = 0;
  x[0] = xinit; x[n] = xfinish;
  b[0] = (2.0 * h * a0 - 3.0 * a1); c[0] = 4.0 * a1;
  a[n] = -4*b1; b[n] = (2*h*b0 + 3*b1);
  d[0] = 2*A*h; d[n] = 2*B*h;

  for(int i = 1; i < n; i++)
  {
    x[i] = x[0] + i * h;
    a[i] = 1.0 - h / 2.0 * p(x[i]);
    b[i] = -(2.0 - h * h * q(x[i]));
	c[i] = 1.0 + h / 2.0 * p(x[i]);
	d[i] = h*h*f(x[i]);
  }

  /*----------Избавимся от y2 в первой строке---*/
  b[0] += a1*a[1]/c[1]; 
  c[0] += a1*b[1]/c[1]; 
  d[0] += a1*d[1]/c[1];
  /*--------------------------------------------*/

  /*----------Избавимся от y(n - 2) в последней строке-----*/
  a[n] += -b1*b[n - 1]/a[n - 1]; 
  b[n] += -b1*c[n - 1]/a[n - 1];
  d[n] += -b1*d[n - 1]/a[n - 1]; 
  /*-------------------------------------------------------*/
  
  solveMatrix (n+1, a, b, c, d, y);

  for(int i = 0; i < n + 1; i++)
  {
    fprintf(file,"%.15lf %.15lf\n", x[i], y[i]);
  }

  /*---Ищем максимальную погрешность на данном шаге---*/
  for(int i = 0; i < n + 1; i++)
  {
    if(fabs(y[i] - (-sin(x[i]))) > max)
	{
	  max = fabs(y[i] - (-sin(x[i])));
	}
  }
  fprintf(file1,"%.15lf %.15lf\n", h, max);
  /*-------------------------------------------------*/
  
  fclose(file); fclose(file1);
  free(a); free(b); free(c); free(x); free(y); free(d);
}



int main(void)
{
  int n = 20;
  char *FileName = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\data.txt";

  for(int i = 2; i < 10; i++)
  {
    n = (int)pow(2.0, i);
    FiniteDifference(FileName, n, 0.0, 1.0, 1.0, 0, 0, 1.0, -sin(0.0), -cos(1.0));
  }

  return 0;
}


