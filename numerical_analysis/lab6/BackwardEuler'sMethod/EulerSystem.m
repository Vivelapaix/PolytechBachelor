function [ts,ys] = Euler(f,tint,y,N)
%  [ts,ys] = Euler(f,[t0,T],y0,N)
%
% Use N steps of Euler method to solve initial value problem
%  y'(t) = f(t,y) , y(t0) = y0
%
% INPUT:
%
% f(t,y)  is function on right hand side of differential equation
%         (returning column vector)
%           for inline function use Euler(f,...)
%           for m-file          use Euler('f',...)
%     t0  is initial time
%      T  is final time
%     y0  is initial value for y at time t0
%      N  is number of Euler steps
%
% OUTPUT:
%
%     ts  is vector of t-values
%     ys  is matrix of y-values: row i contains y-values at time t(i)
%
% EXAMPLE FOR ONE ODE:
%     y' = -y + t,  y(1)=2, find y(t) for t in [1,5] using 20 steps:
%
%     f = inline('-y + t', 't','y')
%     [ts,ys] = Euler(f, [1 5], 2, 20);
%     [ts,ys]              % show table of t and y values
%     plot(ts,ys,'o-')     % plot solution
%
% EXAMPLE FOR SYSTEM OF ODEs:
%     y1' = -2*y1 + y2 + t
%     y2' = y1 - 3*y2, 
%     y1(1) = -1
%     y2(1) = 2 
% Find y(t) for t in [1,5] using 20 steps:
%
%     f = inline('[ -2*y(1) + y(2) + t; y(1) - 3*y(2) ]', 't','y')
%     [ts,ys] = Euler(f, [1 5], [-1;2], 20);
%     [ts,ys]              % show table with columns for t, y1, y2
%     plot(ts,ys,'o-')     % plot solutions y1 and y2
%     legend('y1','y2')

t = tint(1); T = tint(2);  % initial and final t
t0 = t;

ts = zeros(N+1,1);         % initialize ts and ys with zero arrays
ys = zeros(N+1,length(y));

h = (T - t)/N;
ts(1) = t;  % store initial values in arrays ts, ys
ys(1,:) = y.';             % Note: y values are stored as ROW of array ys

for i=1:N
  %s = feval(f,t,y);        % evaluate slope s = f(t,y)
  %y = y + h*s;
  %t = t0 + i*h;
         
  ynew = y + h*feval(f,t,y);
  t = t0 + i*h;
  y = y + h*feval(f,t,ynew); 
  
  ts(i+1,:) = t;         % store new values in arrays ts, ys
  ys(i+1,:) = y.';         % Note: y values are stored as ROW of array ys
end