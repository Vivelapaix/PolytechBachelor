%%
p = @(x) 0;

q = @(x) 0;

f = @(x) sin(x);
solution = @(x) -sin(x);

n = 100;
a0=1;
a1=1;
b0=1;
b1=1;
A = -1;
B = 1;
from = 0;
to = pi;
[x,y] = FiniteDiff1(p,q,f,a0,a1,b0,b1,A,B,from,to,n);
figure
plot(x,y,x, solution(x));

i = 1;
for n = 2:10:502

[x,y] = FiniteDiff1(p,q,f,a0,a1,b0,b1,A,B,from,to,n);
delta(i) = max(abs(solution(x) - y));
i = i+1;
end

i = linspace(2, 502, 51);
figure
loglog(i, i.^(-2), i, delta)








