function M = femass( nodes, weight, rho )
%FEMASS Summary of this function goes here
%   Detailed explanation goes here

M = sparse(size(nodes,2),size(nodes,2));

for i = 1:size(nodes,2)-1
    T    = [nodes(i) nodes(i+1)];
    idx  = [i i+1];
    
    locm = quad_shape_x_shape(T,weight,rho);
    
    for j = 1:2
        for k = 1:2
            M(idx(j),idx(k)) = M(idx(j),idx(k)) + locm(j,k);
        end
    end
end

%%

function M = quad_shape_x_shape(E,w,rho)
%QUAD_SHAPE_X_SHAPE Local quadrature

J = vol1(E);
% Transformation from the reference element
F = @(x) J*x + E(1);
% Shapefunctions on the reference element
lambda = { ( @(x) (1. - x) ) ( @(x) x ) };

% Use the simpson rule
quadp   = [0 0.5 1];
weights = [1  4  1]/6;

M = zeros(2,2);
for p = 1:size(quadp,2)
    for i = 1:2
        for j = 1:2
            M(i,j) = M(i,j) + ...
                J*weights(p)*lambda{i}(quadp(p))*lambda{j}(quadp(p)) * ...
                rho(F(quadp(p)))*w(F(quadp(p)));
        end
   end
end
