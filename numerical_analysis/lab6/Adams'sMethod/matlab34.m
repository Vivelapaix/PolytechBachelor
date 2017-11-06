clc
clear all
close all
% y? =?2x?y
% initial condition
x(1) = 0;
y(1) = -2;
h = 0.1;% step size


%Adam-bashforth 3rd method
%%%%%%%%%%%%%%
for i = 1:10
    f(i) = (-2*x(i)-y(i));
    if(i<3)
        y(i+1) = y(i)+h*(-2*x(i)-y(i));
    else
        y(i+1) = y(i)+h/12*(23*f(i)-16*f(i-1)+5*f(i-2));
        
    end
    x(i+1) = x(i)+h;
end

% plot
figure
set(gca,'Fontsize',15)
plot(x,y,'+-', 'Linewidth', 1.5)
xlabel('x')
ylabel('y')




%ADAM-bashforth 4th method
%%%%%%%%%%%%%%
% yn+1 = (yn -2hxn+1)/(1+h)
for i = 1:10
    f(i) = (-2*x(i)-y(i));
    
    if(i<4)
        y(i+1) = y(i)+h*(-2*x(i)-y(i));
    else
        y(i+1) = y(i)+h/24*(55*f(i)-59*f(i-1)+37*f(i-2)-9*f(i-3));
        
    end
    x(i+1) = x(i)+h;
end



function [ y ] = Adams(f, x0, b, y0, d)
y = zeros(1,d);
y(1) = y0;
h = (b-x0)/(d-1);
for i = 2:1:4 %методом Ёйлера получаем первые четыре точки
y(i) = y(i-1) +h*f(x0+h*(i-1), y(i-1));
end
for i = 5:1:d
y(i) = y(i-1) +h/24 * (55*f(x0+h*(i-2), y(i-1)) - 59*f(x0 + h*(i-3), y(i-2)) +37*f(x0 + h*(i-4), y(i-3)) -9*f(x0 + h*(i-5), y(i-4)));
end

end




% plot
hold on
plot(x,y,'+-', 'Linewidth', 1.5, 'color', 'red')
xlabel('x')
ylabel('y')
legend('3rd order','4th order')