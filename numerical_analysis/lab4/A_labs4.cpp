//*****
//Approximate definite integral of a function f(x) on the interval [a,b]
//using N-points high precision Gauss-Legendre Quadrature.
//*****
#include<stdio.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>

double f(double x)
{
  return //8 + 2*x - x*x // -2 ... 4 // 36
	     //x / sqrt(x*x*x*x + 16) // 0 ... sqrt(3.0) // (log(2.0) / 2.0) 
	     sin(x) // 0 ... M_PI // 2.0
		 ;
}


double GaussQuadrature(char *FileName, double a, double b, double eps, double value, int count, double roots[], double coef[])
{
  FILE *out = NULL;
  double a1 = 0, b1 = 0, h = 0, I1_tmp = 0, I0 = eps, I1 = 0, tmp = (pow(2.0, 2 * count) - 1.0);
  int n = 2;

  fopen_s(&out, FileName, "w");

  for(n = 2; (n <= 4) || (fabs(I0 - I1) / tmp) > eps; n *= 2)
  {
    I0 = I1;
	I1 = 0;
    h = (b - a) / (double)n;
	a1 = 0;
	b1 = h;

	//На каждом из n промежутков вычисляем значение квадратуры
    for(int i = 0; i < n; i++)
	{
	  a1 = a + i * h;
	  b1 = a1 + h;

	  for(int j = 0; j < count; j++)
	  {
	    I1_tmp += coef[j] * f((a1 + b1) / 2.0 + (b1 - a1) / 2.0 * roots[j]);
	  }
	  I1 += I1_tmp * (b1 - a1) / 2.0;

	  I1_tmp = 0;
	}

	fprintf(out,"%i %.15lf\n", n, fabs(I1 - value));
  }

  fclose(out);

  return I1;
}

int main(void)
{
  FILE *froots = NULL, *fcoef = NULL;
  int NumOfRoots = 5;
  double *roots = NULL, *coef = NULL;
  char *FileName = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\data.txt";
  char *FileName1 = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\root.txt";
  char *FileName2 = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\coef.txt";

  fopen_s(&froots, FileName1, "r");
  fopen_s(&fcoef, FileName2, "r");

  roots = (double*)malloc(NumOfRoots * sizeof(double));
  coef = (double*)malloc(NumOfRoots * sizeof(double));

  for(int i = 0; i < NumOfRoots; i++)
  {
    fscanf_s(froots,"%lf", roots + i);
	fscanf_s(fcoef,"%lf", coef + i);
	printf("%lf %lf\n", roots[i], coef[i]);
  }

  printf("\nResult : %.15lf\n", GaussQuadrature(FileName, 0, M_PI, 1e-10, 2.0, NumOfRoots, roots, coef));

  free(roots);
  free(coef);
  fclose(froots);
  fclose(fcoef);

  return 0;
}