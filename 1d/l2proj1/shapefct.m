function [ val ] = shapefct( nnodes, T, node, x )
%SHAPEFCT evaluate shapefunction
%   Detailed explanation goes here

if node == 1
    % left boundary
    E = [T(node) T(node+1)];

    % Debugging
    if(1 ~= inside(E,x))
        val = 0.;
    else
        val = (E(2) - x)/(E(2) - E(1));
    end
elseif node == nnodes
    % right boundary
    E = [T(node-1) T(node)];
    
    % Debugging
    if(1 ~= inside(E,x))
        val = 0.;
    else
        val = (x - E(1))/(E(2) - E(1));
    end
else
    E = [T(node-1) T(node  )];
    F = [T(node  ) T(node+1)];
    
    if 1 == inside(E,x)
        val = (x - E(1))/(E(2) - E(1));
    elseif 1 == inside(F,x)
        val = (F(2) - x)/(F(2) - F(1));
    else
        val = 0.;
    end
end
   
assert(val >= 0);
assert(val <= 1);
