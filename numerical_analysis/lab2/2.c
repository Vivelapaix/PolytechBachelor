#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>
#define pi 3.14159265358979323846
#define a1 0
#define b1 pi
#define K 3 //степень многочлена в мнк

typedef struct splineCoef
{
  double a, b, c, d, x;
} splineCoef;

double func (double x)
{
  return cos(x);
}

splineCoef * buildSpline (splineCoef *spline, double X[], double Y[], int n)
{
  int i;
  double * alpha = (double*)malloc(n * sizeof(double)), *beta = (double*)malloc(n * sizeof(double));//прогоночные коэффициенты
  double A, B, C, F, h_i, h_i1, z;

  for (i = 0; i < n; ++i)
  {
    spline[i].x = X[i];
    spline[i].a = Y[i];
  }
  spline[0].c = 0;
  
  // Решение СЛАУ относительно коэффициентов сплайнов c[i] методом прогонки для трехдиагональных матриц
  // Вычисление прогоночных коэффициентов - прямой ход метода прогонки
  alpha[0] = beta[0] = 0;

  for (i = 1; i < n - 1; ++i)
  {
	h_i = X[i] - X[i - 1];
    h_i1 = X[i + 1] - X[i];
	A = h_i;
	C = 2 * (h_i + h_i1);
	B = h_i1;
	F = 6 * ((Y[i + 1] - Y[i]) / h_i1 - (Y[i] - Y[i - 1]) / h_i);
	z = (A * alpha[i - 1] + C);
	alpha[i] = -B / z;
	beta[i] = (F - A * beta[i - 1]) / z;
  }
  spline[n - 1].c = (F - A * beta[n - 2]) / (C + A * alpha[n - 2]);
 
  // Нахождение решения - обратный ход метода прогонки
  for (i = n - 2; i > 0; --i)
  {
	spline[i].c = alpha[i] * spline[i + 1].c + beta[i];
  }
 
  // Освобождение памяти, занимаемой прогоночными коэффициентами
  free (beta);
  free (alpha);
 
  //По известным коэффициентам c[i] находим значения b[i] и d[i]
  for (i = n - 1; i > 0; --i)
  {
	h_i = X[i] - X[i - 1];
	spline[i].d = (spline[i].c - spline[i - 1].c) / h_i;
	spline[i].b = h_i * (2 * spline[i].c + spline[i - 1].c) / 6 + (Y[i] - Y[i - 1]) / h_i;
  }
  return spline;
}

double cubicSpline (double x, splineCoef *spline, int n)
{
  int i = 0, j = 0, k = 0;
  double dx;
  splineCoef *s = NULL;

  if (x <= spline[0].x) // Если x меньше точки сетки x[0] - пользуемся первым эл-том массива
  {
		s = spline + 1;
  }
  else if (x >= spline[n - 1].x) // Если x больше точки сетки x[n - 1] - пользуемся последним эл-том массива
  {
	s = spline + n - 1;
  }
  else // Иначе x лежит между граничными точками сетки - производим бинарный поиск нужного эл-та массива
  {
	i = 0;
    j = n - 1;
	while (i + 1 < j)
	{
	  k = i + (j - i) / 2;
	  if (x <= spline[k].x)
		j = k;
	  else
		i = k;
	}
	s = spline + j;
  }
  dx = (x - s->x);
  return s->a + (s->b + (s->c / 2 + s->d * dx / 6) * dx) * dx; // Вычисляем значение сплайна в заданной точке.
}



double gridCheb (int i, int n)
{
  return ((a1 + b1)/2 + (b1 - a1)/2 * cos((2 * i + 1) * pi / (2 * (n + 1)))); 
} 

double maxError (double * m, int n)
{
  double max = -1;
  int i;
  for (i = 0; i < n; ++i)
  {
    if (max < fabs(m[i] - func(a1 + i * (double)(a1 + b1)/(n + 5))))
      max =  fabs(m[i] - func(a1 + i * (double)(a1 + b1)/(n + 5)));
  }
  return max;
}

int main (void)
{
  splineCoef * spline = NULL;
  int i, n = 10;
  double *X = NULL, *Y = NULL, x, *y;
  double *A = (double*)malloc((K + 1) * (K + 1) * sizeof(double)), * b = (double*)calloc((K + 1), sizeof(double)), * c = NULL;

  spline = (splineCoef*)malloc(n * sizeof(splineCoef));
  X = (double*)malloc(n * sizeof(double));
  Y = (double*)malloc(n * sizeof(double));
  y = (double*)malloc(2 * n * sizeof(double));

  for (i = 0; i < n; ++i)
  {
    X[i] = a1 + i * (double)(a1 + b1)/n;
    //X[i] = gridCheb (i, n);
    Y[i] = func(X[i]);
  }

  spline = buildSpline(spline, X, Y, n);

  for (i = 0; i < n + 5; ++i)
  {
    x = a1 + i * (double)(a1 + b1)/(n + 5);
    //x = gridCheb (i, n + 5);
    y[i] = cubicSpline (x, spline, n);
    //fprintf (f, "%.15f %.15f\n", x, y);
    }

    

    /*free(b);
    free(A);
    free(c);
    free(X);
    free(Y);
    free(y);
    free(spline);*/
  //}
  return 0;
}
