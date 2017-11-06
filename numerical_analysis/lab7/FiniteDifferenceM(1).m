function y = FiniteDifferenceM(x, A, B)
n = length(x);
M = zeros(n);
R = zeros(n, 1);
h = x(2) - x(1);

% first string of matrix
M(1,1) = 1;
R(1) = A;

% 2 -- n-1 strings
for i = 2: n-1
    M(i,i-1) = 1 - h/2 * p(x(i));
    M(i,i) = h.^2 * q(x(i)) - 2;
    M(i,i+1) = 1 + h/2 * p(x(i));
    R(i) = h.^2 * f(x(i));
end

% n+1 -- the last string
M(n,n-2) = 1;
M(n,n-1) = -4;
M(n,n) = 3;
R(n) = 2*h*B;

% solve the system
solution = M \ R;
y = solution';
end