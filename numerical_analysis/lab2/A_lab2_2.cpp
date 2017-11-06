#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>


void GaussMethod(double **a, double *b, double *x, int n)
{
  int i = 0, j = 0, k = 0, max = 0;
  double temp = 0, tmp = 0, sum = 0;

  for(k = 0; k < n ; k++)
	{
		max = k;
		for(i = k + 1; i < n; ++i)
		{
			if(fabs(a[max][k]) < fabs(a[i][k]))
				max = i;
		}
		if(fabs(a[max][k]) < 1e-16)
		{
			printf("NO");
		}
		else
		{
			temp = b[k];
			b[k] = b[max];
			b[max] = temp;
			for(j = k; j < n; ++j)
			{
				temp = a[k][j];
				a[k][j] = a[max][j];
				a[max][j] = temp;
			}
		}
		for(i = k + 1; i < n; i++)
		{
			tmp = a[i][k] / a[k][k];
			b[i] = b[i] - tmp * b[k];
			for(j = k + 1; j < n; j++)
			{
				a[i][j] = a[i][j] - tmp * a[k][j];
			}
		}
	}
	x[n-1] = b[n-1] / a[n-1][n-1];
	for(k = n-2;k >= 0; k--)
	{
		sum = 0;
		for(j = k + 1;j < n;j++)
			sum += a[k][j] * x[j];
		x[k]=(b[k] - sum) / a[k][k];
	}
 
	printf("\n---------------Solution\n");
	for(k = 0; k < n ; k++)
		printf("%lf\n", x[k]);
	printf("-----------------Solution\n");
}

double MyPolynom(double x, double *Coef)
{
  return Coef[0] + Coef[1]*x + Coef[2]*x*x + Coef[3]*x*x*x;
}

void PolynomGrid(const char* FileName, const double a, const double b, const int n, double *Coef)
{
  FILE *file = NULL;
  double step = (b - a) / (n - 1);
  int i = 0;

  fopen_s(&file, FileName, "w");

  for(i = 0; i < n; i++)
  {
    fprintf(file,"%.15lf %.15lf\n", a + i * step, MyPolynom(a + i * step, Coef));
  }

  fclose(file);
}

void CalculWeights(const double *X, const double *Y, double *Coef, int n, double *Weight)
{
  int j = 0;

  for(j = 0; j < n; ++j)
  {
    Weight[j] = 1.0 / (double)((MyPolynom(X[j], Coef) - Y[j]) * (MyPolynom(X[j], Coef) - Y[j]) + 1e-9);
  }
}

void WeightedLeastSquares(int m, int n, int w, const double *X, const double *Y, double *Coef)
{
  /* m - the degree of the polynomial */
  /* n - the number of grid nodes */
  /* m < n */
  double *PowerBasis = NULL, *FreeColumn = NULL, *Weight = NULL;
  double **matrix;
  int i = 0, j = 0, k = 0, count = 0;

  printf("\nMY---COEFFICIENTS %lf %lf %lf %lf\n",Coef[0], Coef[1], Coef[2], Coef[3]);

  PowerBasis = (double*)calloc(2 * m + 1, sizeof(double));
  FreeColumn = (double*)calloc(m + 1, sizeof(double));
  matrix = (double**)calloc(m + 1, sizeof(double*));

  if(w)
  {
    Weight = (double*)calloc(n, sizeof(double));
    CalculWeights(X, Y, Coef, n, Weight);
  }

  for(i = 0; i < m + 1; ++i)
  {
	matrix[i] = (double*)calloc(m + 1, sizeof(double));
  }

  /* Строим степенной базис */
  for(i = 0; i < 2 * m + 1; ++i)
  {
    for(j = 0; j < n; ++j)
	{
	  if(!w)
	  {
        PowerBasis[i] += pow(X[j], i);
	  }
	  else
	  {
	    PowerBasis[i] += pow(X[j], i) * Weight[j];
	  }
	}  
  }

  /*Заполняем матрицу для решения*/
  for(i = 0; i < m + 1; ++i)
  {
    k = count;
    for(j = 0; j < m + 1; ++j)
	{
	  matrix[i][j] = PowerBasis[k++];
	  printf("%lf ", matrix[i][j]);
	}
	printf("\n");
	count++;
  }

  for(i = 0; i < m + 1; ++i)
  {
    for(j = 0; j < m + 1; ++j)
	{
	  printf("%lf ", matrix[i][j]);
	}
	printf("\n");
  }

  /*Заполняем столбец свободных членов*/
  for(i = 0; i < m + 1; ++i)
  {
    for(j = 0; j < n; ++j)
	{
	  if(!w)
	  {
	    FreeColumn[i] += pow(X[j], i) * Y[j];
	  }
	  else
	  {
        FreeColumn[i] += pow(X[j], i) * Y[j] * Weight[j];
	  }
	}
	printf("%lf\n", FreeColumn[i]);
  }
  
  GaussMethod(matrix, FreeColumn, Coef, m + 1);

  for(i = 0; i < m + 1; ++i)
  {
	free(matrix[i]);
  }
  free(matrix);
  free(PowerBasis);
  free(FreeColumn);
  if(w)
  {
    free(Weight);
  }
}

double Polynom(double x)
{
  return (x - 2)*(x - 4)*(x - 7);
}

void CreateGridToFile(const char* FileName, const double a, const double b, const int n, const int weight, double X[], double Y[])
{
  FILE *file = NULL;
  double step = (b - a)/(n - 1);
  int i = 0;
  
  assert(NULL != FileName);
 
  fopen_s(&file, FileName, "w");

  for(i = 0; i < n; i++)
  {
    X[i] = a + i * step;
	
	if(weight == 0)
	{
	  Y[i] = Polynom(X[i]);
	}
	else if(weight == 1)
	{
	  Y[i] = Polynom(X[i])+ 5*rand()/RAND_MAX - 5;
	}
	else
	{
	  Y[i] = Polynom(X[i])+ 5*rand()/RAND_MAX - 5;
	  if((i % 10) == 0)
	    Y[i] = 30;
	}
	fprintf(file,"%.15lf %.15lf\n", X[i], Y[i]);
  }
  fclose(file);
}

void ReadGridFromFile(const char* FileName, const int n, double X[], double Y[])
{
  FILE *file = NULL;
  int i = 0;

  assert(NULL != FileName);
 
  fopen_s(&file, FileName, "r");

  for(i = 0; i < n; i++)
  {
    fscanf_s(file,"%lf %lf", X + i, Y + i);
  }
  fclose(file);
}


int main(void)
{
  int n = 10, PolyDegree = 3;
  double a = 1.0, b = 8.0;
  double *X = NULL, *Y = NULL, *Coef = NULL;
  char *CreateNewGrid = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\data.txt";
  char *CreateMyPolyGrid = "C:\\Файлы\\Численные методы\\MATLAB\\R2008a\\bin\\poly.txt";

  n = 100;
  X = (double*)malloc(n * sizeof(double));
  Y = (double*)malloc(n * sizeof(double));
  Coef = (double*)calloc(PolyDegree + 1, sizeof(double));

  CreateGridToFile(CreateNewGrid, a, b, n, 1, X, Y);
  WeightedLeastSquares(PolyDegree, n, 0, X, Y, Coef);
  WeightedLeastSquares(PolyDegree, n, 1, X, Y, Coef);
  PolynomGrid(CreateMyPolyGrid, a, b, 200, Coef);

  free(X);
  free(Y);
  free(Coef);

 return 0;
}