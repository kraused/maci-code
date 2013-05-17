function test
%TEST Summary of this function goes here
%   Detailed explanation goes here

% Test treshold values
eps1 = 1e-12;    %test1: 1*M*1 - vol
eps2 = 1e-12;    %test2: det(M)
eps4 = 1e-12;    %test4: subs
eps7 = 1e-11;    %testx: 1*R*1 - vol
eps8 = 1e-12;    %testx: 1. - norm(P*1)

% MD lattice
req  = 2^(1/6);
A    = req*(-200: 1:200);

% FE mesh
c4n  = req*(-200:10:200);

% Density function
rho  = @(x) ones(size(x));

% PU diam
h    = 0.75*req;
% Construct subs
[nbh cuts subs] = ConstructSubs(A,5,h);

% Check neighbour list
if 2 ~= size(nbh,2) || 4 ~= nbh(1) || 6 ~= nbh(2)
    str = sprintf('test10: failed\n');
else
    str = sprintf('test10: passed\n');
end
disp(str)

% Check vol
vol = 0.;
for i = 1:size(subs,1)
    vol = vol + vol1(subs(i,:));
end
d = abs(vol - 2*h);
if d > eps4
    str = sprintf('test11: failed (%16.15e)\n', d);
else
    str = sprintf('test11: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 0.95*req;
% Construct subs
[nbh cuts subs] = ConstructSubs(A,5,h);

% Check neighbour list
if 2 ~= size(nbh,2) || 4 ~= nbh(1) || 6 ~= nbh(2)
    str = sprintf('test03: failed\n');
else
    str = sprintf('test03: passed\n');
end
disp(str)

% Check vol
vol = 0.;
for i = 1:size(subs,1)
    vol = vol + vol1(subs(i,:));
end
d = abs(vol - 2*h);
if d > eps4
    str = sprintf('test04: failed (%16.15e)\n', d);
else
    str = sprintf('test04: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 1.50*req;
% Construct subs
[nbh cuts subs] = ConstructSubs(A,5,h);

% Check neighbour list
if 4 ~= size(nbh,2) || 3 ~= nbh(1) || 4 ~= nbh(2) || 6 ~= nbh(3) || 7 ~= nbh(4)
    str = sprintf('test05: failed\n');
else
    str = sprintf('test05: passed\n');
end
disp(str)

% Check vol
vol = 0.;
for i = 1:size(subs,1)
    vol = vol + vol1(subs(i,:));
end
d = abs(vol - 2*h);
if d > eps4
    str = sprintf('test06: failed (%16.15e)\n', d);
else
    str = sprintf('test06: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 2.50*req;
% Construct subs
[nbh cuts subs] = ConstructSubs(A,5,h);

% Check neighbour list
if 8 ~= size(nbh,2) || ...
    1 ~= nbh(1) || 2 ~= nbh(2) || 3 ~= nbh(3) || 4 ~= nbh(4) || ...
    6 ~= nbh(5) || 7 ~= nbh(6) || 8 ~= nbh(7) || 9 ~= nbh(8)
    str = sprintf('test05: failed\n');
else
    str = sprintf('test05: passed\n');
end
disp(str)

% Check vol
vol = 0.;
for i = 1:size(subs,1)
    vol = vol + vol1(subs(i,:));
end
d = abs(vol - 2*h);
if d > eps4
    str = sprintf('test07: failed (%16.15e)\n', d);
else
    str = sprintf('test07: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 0.75*req;
% Compute projection
[P M R] = l2proj1(h,A,c4n,rho);

% Check if mass matrix gives the right volume
d = abs(ones(1,size(M,1))*M*ones(size(M,2),1) - (size(A,2)-1)*req);
if d > eps1
    str = sprintf('test14: failed (%16.15e)\n', d);
else
    str = sprintf('test14: passed (%16.15e)\n', d);
end
disp(str)

% Check if mass matrix is invertible
if det(M) < eps2
    str = sprintf('test15: failed (%16.15e)\n', det(M));
else
    str = sprintf('test15: passed (%16.15e)\n', det(M));
end
disp(str)   

% Check if the rectangular matrix gives the right volume
d = abs(ones(1,size(R,1))*R*ones(size(R,2),1) - (size(A,2)-1)*req);
if d > eps7
    str = sprintf('test16: failed (%16.15e)\n', d);
else
    str = sprintf('test16: passed (%16.15e)\n', d);
end
disp(str)

% Check the conversation of unity property
d = abs(norm(P*ones(size(P,2),1) - ones(size(P,1),1)));
if d > eps8
    str = sprintf('test17: failed (%16.15e)\n', d);
else
    str = sprintf('test17: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 0.95*req;
% Compute projection
[P M R] = l2proj1(h,A,c4n,rho);

% Check if mass matrix gives the right volume
d = abs(ones(1,size(M,1))*M*ones(size(M,2),1) - (size(A,2)-1)*req);
if d > eps1
    str = sprintf('test01: failed (%16.15e)\n', d);
else
    str = sprintf('test01: passed (%16.15e)\n', d);
end
disp(str)

% Check if mass matrix is invertible
if det(M) < eps2
    str = sprintf('test02: failed (%16.15e)\n', det(M));
else
    str = sprintf('test02: passed (%16.15e)\n', det(M));
end
disp(str)    

% Check if the rectangular matrix gives the right volume
d = abs(ones(1,size(R,1))*R*ones(size(R,2),1) - (size(A,2)-1)*req);
if d > eps7
    str = sprintf('test18: failed (%16.15e)\n', d);
else
    str = sprintf('test18: passed (%16.15e)\n', d);
end
disp(str)

% Check the conversation of unity property
d = abs(norm(P*ones(size(P,2),1) - ones(size(P,1),1)));
if d > eps8
    str = sprintf('test19: failed (%16.15e)\n', d);
else
    str = sprintf('test19: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 1.50*req;
% Compute projection
[P M R] = l2proj1(h,A,c4n,rho);

% Check if mass matrix gives the right volume
d = abs(ones(1,size(M,1))*M*ones(size(M,2),1) - (size(A,2)-1)*req);
if d > eps1
    str = sprintf('test12: failed (%16.15e)\n', d);
else
    str = sprintf('test12: passed (%16.15e)\n', d);
end
disp(str)

% Check if mass matrix is invertible
if det(M) < eps2
    str = sprintf('test13: failed (%16.15e)\n', det(M));
else
    str = sprintf('test13: passed (%16.15e)\n', det(M));
end
disp(str)    

% Check if the rectangular matrix gives the right volume
d = abs(ones(1,size(R,1))*R*ones(size(R,2),1) - (size(A,2)-1)*req);
if d > eps7
    str = sprintf('test20: failed (%16.15e)\n', d);
else
    str = sprintf('test20: passed (%16.15e)\n', d);
end
disp(str)

% Check the conversation of unity property
d = abs(norm(P*ones(size(P,2),1) - ones(size(P,1),1)));
if d > eps8
    str = sprintf('test21: failed (%16.15e)\n', d);
else
    str = sprintf('test21: passed (%16.15e)\n', d);
end
disp(str)

% PU diam
h    = 2.50*req;
% Compute projection
[P M R] = l2proj1(h,A,c4n,rho);

% Check if mass matrix gives the right volume
d = abs(ones(1,size(M,1))*M*ones(size(M,2),1) - (size(A,2)-1)*req);
if d > eps1
    str = sprintf('test08: failed (%16.15e)\n', d);
else
    str = sprintf('test08: passed (%16.15e)\n', d);
end
disp(str)

% Check if mass matrix is invertible
if det(M) < eps2
    str = sprintf('test09: failed (%16.15e)\n', det(M));
else
    str = sprintf('test09: passed (%16.15e)\n', det(M));
end
disp(str)  

% Check if the rectangular matrix gives the right volume
d = abs(ones(1,size(R,1))*R*ones(size(R,2),1) - (size(A,2)-1)*req);
if d > eps7
    str = sprintf('test22: failed (%16.15e)\n', d);
else
    str = sprintf('test22: passed (%16.15e)\n', d);
end
disp(str)

% Check the conversation of unity property
d = abs(norm(P*ones(size(P,2),1) - ones(size(P,1),1)));
if d > eps8
    str = sprintf('test23: failed (%16.15e)\n', d);
else
    str = sprintf('test23: passed (%16.15e)\n', d);
end
disp(str)
