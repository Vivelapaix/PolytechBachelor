function Dy = runge(x, y)
Dy = y(:);
Dy(1) = y(2);
Dy(2) = cos(3*x) - 4*y(1);