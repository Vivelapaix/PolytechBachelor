H = []
Y = []

for i=2:2:16
    n = 8^i;
  h = 1/n;
  H = [H h]
  %Y = [Y abs(-sin(0.5)-(sin(0.5 - h) - 2*sin(0.5)+ sin(0.5 + h))/h^2)]  
  Y = [Y abs(2-((0.5 - h)^2 - 2*(0.5)^2+ (0.5 + h)^2)/h^2)]
end

loglog(H, Y, 'm')
grid on



%-------------------------------------------
%����� �������� ��������� (��������� �������)
l=load('-ascii', 'data.txt')
k = l(:, 2)
e = l(:, 1)
plot(e, k, 'm')
hold on
grid on

%���������� �������
x = 0 : 0.01 : e(length(e));
y = -sin(x);
plot(x, y, 'r')
hold on
grid on
%-------------------------------------------



%-------------------------------------------
%������ �������� ������
l1=load('-ascii', 'err.txt')
k1 = l1(:, 2)
e1 = l1(:, 1)
loglog(e1, k1, 'm')
hold on
grid on

y1 = (e1.*e1);
loglog(e1, y1, 'r')
hold on
grid on
%-------------------------------------------


%dsolve('D2y + x*x*y - x*y = (6.0/x*x*x*x - 3.0/x)', 'y(1) = 1, y(2) = 0.25')