function test
%TEST Summary of this function goes here
%   Detailed explanation goes here

eps1 = 1e-12;
eps2 = 1e-12;

% FE mesh
h    = 0.125;
c4n  = h*(-200:200);

% weight
w    = @(x) ones(size(x));
% rho
rho  = @(x) ones(size(x));

M = femass(c4n, w, rho);

% Test conversation of unity
d = abs(ones(1,size(M,1))*M*ones(size(M,2),1) - (size(c4n,2)-1)*h);
if d > eps1
    str = sprintf('test01: failed (%16.15e)\n', d);
else
    str = sprintf('test01: passed (%16.15e)\n', d);
end
disp(str)

F = fecybnf(c4n, ones(size(c4n)), 1., 1., 2^(1/6), w);

% Test stability
d = norm(F);
if d > eps2
    str = sprintf('test02: failed (%16.15e)\n', d);
else
    str = sprintf('test02: passed (%16.15e)\n', d);
end
disp(str)

V = fecybne(c4n, ones(size(c4n)), 1., 1., 2^(1/6), w);

% Test stability
if V >= 0.
    str = sprintf('test03: failed (%16.15e)\n', V);
else
    str = sprintf('test03: passed (%16.15e)\n', V);
end
disp(str)

