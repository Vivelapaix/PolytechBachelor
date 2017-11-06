#include<stdio.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>


double IntegralFunction(double x)
{
  return /*x * x*/x*x*x/*1.0/sqrt(x)*//*1.0 / sqrt(x + sqrt(2.0))*/  ///х^3 хорошо приближает? Ошибка 0 Почему?:  график зависимости ошибки от шага для гладкой и негладкой функции; 1? 1/sqrt(x - sqrt(2))
	/*  fabs(x)*//*sin(x)*/;
}


double SimpsonMethod(char *FileName, double a, double b, double eps, double value)
{
  FILE *file = NULL;
  int i = 0, k = 2;
  double h = 0, chet = 0, nechet = 0, I = 0, I0 = 0;
  double *X = NULL, *Y = NULL;

  fopen_s(&file, FileName, "w");

  for(k = 2; (k <= 4) || (fabs(I - I0) > eps); k *= 2)
  {
    I0 = I;
	  chet = nechet = I = 0;
    h = (b - a) / k;
    
    X = (double*)malloc((k + 1) * sizeof(double));
    Y = (double*)malloc((k + 1) * sizeof(double));

    for(i = 0; i < k + 1; i++)
    {
      X[i] = a + i * h;
	    Y[i] = IntegralFunction(X[i]);
      printf("%i %lf %lf\n", i, X[i], Y[i]);
    }

    for(i = 1; i < k; i++)
    {
      if(i % 2 == 0)
	  {
	    chet += Y[i];
	  }
	  else
	  {
	    nechet += Y[i];
	  }
    }
	I = h / 3 * (Y[0] + 2 * chet + 4 * nechet + Y[k]);

  fprintf(file, "%i %.15lf\n", k, fabs(I - value));

	free(X);
	free(Y);
  }
  fclose(file);
  return I0;
}

int main(void)
{
  char *FileName = "C:\\Users\\user_phmf6\\Documents\\MATLAB\\data2.txt";
  printf("%.15lf", SimpsonMethod(FileName, -1, 1, 1e-5, 0.5));
  return 0;
}