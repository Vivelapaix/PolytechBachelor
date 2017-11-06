#include <stdio.h>
#include <math.h>

double df(double x, double y)
{
  //return x + y;
  return 5*(y - x*x);
}

void Runge–KuttaMethod4th(char *FileName, const double a, const double b, const double y0, const int n)
{
  FILE *file = NULL;
  int i = 0, j = 0;
  double c[4] = {0, 0.5, 0.5, 1.0}, k[5] = {0};
  double x = a, h = (b - a) / (double) n, y = y0;

  fopen_s(&file, FileName, "w");

  for (i = 1; i <= n + 1; ++i)
  {
    for (j = 1; j <= 4; ++j)
    {
      k[j] = h * df(x + c[j - 1] * h, y + c[j - 1] * k[j - 1]);
    }
    fprintf(file, "%.15lf %.15lf\n", x, y);
    y += (k[1] + 2 * k[2] + 2 * k[3] + k[4]) / 6.0;
    x += h;
  }
  fclose(file);
}

int main (void)
{
  char *FileName = "C:\\Ôàéëû\\×èñëåííûå ìåòîäû\\MATLAB\\R2008a\\bin\\RK1.txt";

  Runge–KuttaMethod4th(FileName, 0, 1.0, 0.08, 50);
  
  return 0;
}