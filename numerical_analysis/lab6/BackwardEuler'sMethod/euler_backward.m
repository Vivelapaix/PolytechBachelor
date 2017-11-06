function [x, y] = euler_backward(f, xinit, yinit, xfinal, n)

h = (xfinal - xinit)/n;
x = [xinit zeros(1,n)];
y = [yinit zeros(1,n)];

for i = 1 : n
  x(i + 1) = x(1) + i*h;
  ynew = y(i) + h*(f(x(i), y(i)));
  y(i + 1) = y(i) +h*f(x(i + 1), ynew);
end

end