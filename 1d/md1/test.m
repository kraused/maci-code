function test
%TEST Summary of this function goes here
%   Detailed explanation goes here

eps1 = 1e-10;

% MD lattice
req  = 2^(1/6);
A    = req*(-200: 1:200);

% weight
w    = @(x) ones(size(x));

F = ljf(A, 1.5, 1., 1., w(A));

% Test forces
d = norm(F);
if d > eps1
    str = sprintf('test01: failed (%16.15e)\n', d);
else
    str = sprintf('test01: passed (%16.15e)\n', d);
end
disp(str)

V = lje(A, 1.5, 1., 1., w(A));

% Potential
if V >= 0.
    str = sprintf('test02: failed (%16.15e)\n', V);
else
    str = sprintf('test02: passed (%16.15e)\n', V);
end
disp(str)
