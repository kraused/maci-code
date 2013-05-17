function N = interpol( atoms, nodes )
%INTERPOL Construct interpolation matrix
%   Detailed explanation goes here

N = sparse(size(atoms,2),size(nodes,2));
for i = 1:size(nodes,2)
    o = fepatch(nodes,i);
    
    for j = 1:size(atoms,2)
        if inside(o,atoms(j))
            N(j,i) = hat(o,nodes(i),atoms(j));
        end
    end
end
