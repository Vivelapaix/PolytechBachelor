syms x n
n = 5
f1 = (1/(factorial(n)*2^(n)))
f = f1*diff((x^2-1)^n, n)
expand(f)
root = roots(sym2poly(f))

%Сортировка корней
root = sort(root)

%Заполнение матрицы и свободного столбца
for i = 1 : n
  for j = 1 : n
     A(i, j) = root(j).^(i - 1);  
  end
  if (mod(i - 1, 2) == 0)
    b(i) = 2./(i);
  else
    b(i) = 0; 
  end
end   

%Решение системы для нахождения коэффициентов(весов)
coef = A\b';
%Запись в файл
save('coef.txt','coef', '-ascii','-double');
save('root.txt','root', '-ascii','-double');


x=load('-ascii', 'data.txt')
k = x(:, 2)
e = x(:, 1)
loglog(e, k, 'm')
hold on
grid on

x = 2 : 0.01 : e(length(e))
y = 1./(x.^(2*n))
loglog(x, y, 'r')
