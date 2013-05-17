function test
%TEST Test interpolation matrix
%   Detailed explanation goes here

eps1 = 1e-12;

% MD lattice
req  = 2^(1/6);
A    = req*(-200: 1:200);

% FE mesh
c4n  = req*(-200:10:200);

N = interpol(A, c4n);

% Test conversation of unity
d = abs(norm(N*ones(size(N,2),1) - ones(size(N,1),1)));
if d > eps1
    str = sprintf('test01: failed (%16.15e)\n', d);
else
    str = sprintf('test01: passed (%16.15e)\n', d);
end
disp(str)
