function value = feval1( c4n, u, x )
%FEVAL1 Summary of this function goes here
%   Detailed explanation goes here

for i = 1:size(c4n,2)-1
	if inside([c4n(i) c4n(i+1)], x)
		value = locvalue([c4n(i) c4n(i+1)], [u(i) u(i+1)], x);
		break;
	end
end

%%

function value = locvalue(E, u, x)
%LOCVALUE

J = vol1(E);
F = @(x) (x - E(1))/J;

value = (1 - F(x))*u(1) + F(x)*u(2);
