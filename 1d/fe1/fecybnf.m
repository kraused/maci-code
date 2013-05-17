function FF = fecybnf( nodes, u, sigma, eps, req, weight )
%FECYBNF Summary of this function goes here
%   Detailed explanation goes here

% FIXME: For performance reasons WEIGHT is no longer a function handle but
%	 the weight values at the nodes and I'm using linear interpolation
%	 between these values.

FF = zeros(size(nodes)); 

% piola tensor
piola = @(F) 24*eps*(  ((sigma)^6 )/((F*req)^7 ) - ...
                     2*((sigma)^12)/((F*req)^13));
	
% simpson
quadp   = [0 0.5 1];
weights = [1  4  1]/6;
 
for i = 1:size(nodes,2)
    if i ~= 1
        E    = [nodes(i-1) nodes(i)];
	J    = E(2) - E(1);
        F    = 1 + (u(i) - u(i-1))/J;
        P    = piola(F);
        grad = 1/J;
        
	w    = [ weight(i-1) 0.5*(weight(i-1)+weight(i)) weight(i) ];
	
        for p = 1:size(quadp,2)
            FF(i) = FF(i) - J*weights(p)*P*grad*w(p);
        end
    end

    if i ~= size(nodes,2)
        E    = [nodes(i) nodes(i+1)];
        J    = E(2) - E(1);
        F    = 1 + (u(i+1) - u(i))/J;
        P    = piola(F);
        grad = 1/(-J);
	
	w    = [ weight(i) 0.5*(weight(i)+weight(i+1)) weight(i+1) ];
	
        for p = 1:size(quadp,2)
            FF(i) = FF(i) - J*weights(p)*P*grad*w(p);
        end
    end
end
