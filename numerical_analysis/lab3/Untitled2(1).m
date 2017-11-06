figure
%������ �������� �-���
x = [0.1 0.3 0.45 0.5 0.79 1.1 1.89 2.4 2.45];
y = [-3 -1 0.9 2.4 2.5 1.9 0.1 -1.3 -2.6];

%������ ��������
p1 = polyfit(x, y, 1)
p3 = polyfit(x, y, 3)
p5 = polyfit(x, y, 5)

%���������� 100 �����, ���������� ������������� �� ������� ����������� ������, ��� ������ ������� linspace

xx = linspace(x(1), x(end), 100);

%�������� �������� ���������  ��� ������ ������� polyval 
yy1 = polyval(p1, xx);
yy3 = polyval(p3, xx);
yy5 = polyval(p5, xx);

plot(x, y, 'o', xx, yy1, xx, yy3, xx, yy5)




x = 0:pi/2:4*pi
y = cos(x)
p1 = csape(x, y)%���������� ������ ����������� ������� � ����������� �������� ������� �������, ���������� ����� ������ ����� � ��������� ���
figure
fnplt(p1)
hold on
fplot (@cos, [0 4*pi], 'r')
hold on
plot (x, y, 'ko')



p2 = csape(x, y, 'periodic') %������������� ��������� �������, �������� ������ � ������ ����������� ������� � ����� �� ������� ����� ������� ��������� � �����.���������� ����������� � ������ ������� ����� �������
figure
fnplt(p2)
hold on
fplot (@cos, [0 4*pi], 'g')
hold on
plot (x, y, 'ko')
legend ('periodic')

p3 = csape(x, y, 'variational') %������������ ������, ������� ������ �����������
figure
fnplt(p3)
hold on
fplot (@cos, [0 4*pi], 'k')
hold on
plot (x, y, 'ko')
legend ('variational')


p4 = csape(x, y, 'not-a-knot')%� �������� ���������� ����, ������ ����������� �������� ����������
figure
fnplt(p4)
hold on
fplot (@cos, [0 4*pi], 'm')
hold on
plot (x, y, 'k*')
legend ('not-a-knot')








