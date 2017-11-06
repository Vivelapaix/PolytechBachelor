figure
%задаем таблично ф-цию
x = [0.1 0.3 0.45 0.5 0.79 1.1 1.89 2.4 2.45];
y = [-3 -1 0.9 2.4 2.5 1.9 0.1 -1.3 -2.6];

%строим полиномы
p1 = polyfit(x, y, 1)
p3 = polyfit(x, y, 3)
p5 = polyfit(x, y, 5)

%генерируем 100 точек, равномерно расположенных на области определения данных, при помощи функции linspace

xx = linspace(x(1), x(end), 100);

%вычислим значения полиномов  при помощи функции polyval 
yy1 = polyval(p1, xx);
yy3 = polyval(p3, xx);
yy5 = polyval(p5, xx);

plot(x, y, 'o', xx, yy1, xx, yy3, xx, yy5)




x = 0:pi/2:4*pi
y = cos(x)
p1 = csape(x, y)%совпадение первой производной сплайна с производной полинома третьей степени, проходящей через данную точку и ближайшие три
figure
fnplt(p1)
hold on
fplot (@cos, [0 4*pi], 'r')
hold on
plot (x, y, 'ko')



p2 = csape(x, y, 'periodic') %периодические граничные условия, значения первой и второй производной сплайна в одной из крайних точек отрезка совпадают с соотв.значениями производных в другой крайней точке отрезка
figure
fnplt(p2)
hold on
fplot (@cos, [0 4*pi], 'g')
hold on
plot (x, y, 'ko')
legend ('periodic')

p3 = csape(x, y, 'variational') %естественный сплайн, нулевая вторая производная
figure
fnplt(p3)
hold on
fplot (@cos, [0 4*pi], 'k')
hold on
plot (x, y, 'ko')
legend ('variational')


p4 = csape(x, y, 'not-a-knot')%с условием отсутствия узла, третья производная сплайнов непрерывна
figure
fnplt(p4)
hold on
fplot (@cos, [0 4*pi], 'm')
hold on
plot (x, y, 'k*')
legend ('not-a-knot')








