function o = fepatch( nodes, k )
%FEPATCH Patch of FE node
%   Detailed explanation goes here

if k == 1
    o = [nodes(k  ) nodes(k+1)];
elseif k == size(nodes,2)
    o = [nodes(k-1) nodes(k  )];
else
    o = [nodes(k-1) nodes(k+1)];
end
