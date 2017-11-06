a = 0;
b = 1;
n = 1000
h =(b - a) / n;
x(1) = a;
y(1) = 0.08;
for i = 1 : n
x(i+1) = x(i) + h;
y(i+1) = y(i) + h * dx(x(i),y(i));
end;
V=[x',y']
plot(x, y, 'r')
grid on
hold on

r=load('-ascii', 'RK1.txt')
k = r(:, 2)
e = r(:, 1)
plot(e, k, 'b')
hold on
grid on

x1 = a: 0.001 : b;
y1 = x1.^2 + 0.4.*x1 + 0.08;
plot(x1, y1, 'g')
grid on
hold on


