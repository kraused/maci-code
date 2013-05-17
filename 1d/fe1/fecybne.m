function V = fecybne( nodes, u, sigma, eps, req, weight )
%FECYBNE Summary of this function goes here
%   Detailed explanation goes here

% stored energy density
wcb = @(F) 4*eps*(sigma/(F*req))^6*((sigma/(F*req))^6 - 1)/req;

V = 0.;
for i = 1:size(nodes,2)-1
    E = [nodes(i) nodes(i+1)];
    F = 1 + (u(i+1) - u(i))/(E(2) - E(1));

    % Simpson rule (F is constant and alpha linear)
    quadp   = [ 0 0.5 1 ];
    weights = [ 1 4 1 ]/6;
    J       = E(2) - E(1);

     for p = 1:size(quadp,2)
         V = V + J*weights(p)*weight(J*quadp(p) + E(1))*wcb(F);
     end
end
