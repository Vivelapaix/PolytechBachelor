#include<stdio.h>

double f (double x1)
{
  return (1/(x1+16.7))+x1+3; //F(x)=1/(x+16.7) + x + 3
}
int main (void)
{
  double b, h, k1, k2, k3, k4, y[100], x[100];
  int k;

  h = 0.05; //шаг
  x[0] = 1.5; //начало интервала и нач. условие х
  b = 5; //конец интервала
  y[0] = 1.4; //нач. условие y
  for (k = 0; k <= 3; ++k)
  {
    k1 = h * f(x[k]);
    k2 = h*f(x[k] + h/2);
    k3 = h*f(x[k] + h/2);
    k4 = h*f(x[k] + h);
    y[k+1] = y[k] + (k1 + 2 * k2 + 2 * k3 + k4) / 6; // метод Рунге-Кутта
    x[k+1] = x[k] + h;
    printf ("%lf, %lf\n", x[k], y[k]);
  }
  for (k = 4; k < 100; ++k)
  {
    if (x[k] >= (b + h / 2))
      break;
    y[k+1] = y[k] + (h / 24) * (55 * f(x[k]) - 59 * f(x[k-1]) + 37 * f(x[k-2]) - 9 * f(x[k-3]));
    printf ("%lf, %lf\n", x[k], y[k]);
    x[k+1] = x[k] + h;
  }
}