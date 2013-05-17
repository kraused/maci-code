function [nbh,cuts,subs] = ConstructSubs(atoms,k,h)
%CONSTRUCT_SUBS Resolve discontinuities in the gradient of PU basis
%               functions
%   Detailed explanation goes here

o0 = pupatch(atoms(k),h);

nbh = [];   % Neighbour pupatches
for i = 1:size(atoms,2)
    if i == k
        continue
    end
    
    o1 = pupatch(atoms(i),h);
    
    if vol1(cut(o0,o1)) > 1e-12
        nbh(size(nbh,2)+1) = i;
    end
    
    % I can't handle this case
    assert(0 == subset(o0,o1) && 0 == subset(o1,o0));
end

cuts = o0;
ii = 1;
for i = 1:size(nbh,2)
    o1 = cut(o0,pupatch(atoms(nbh(i)),h));
    if vol1(o1) < 1e-12
        continue
    end
    
    ii = ii+1;
    cuts(ii,:) = o1;
end

% constructing the subs in 1d is very easy. We just need to construct the
% set of all endpoints, sort the set and read out the intervalls

ep = [];
ii = 1;
for i = 1:size(cuts,1)
    ep(ii,:) = cuts(i,1);
    ii = ii + 1;
    ep(ii,:) = cuts(i,2);
    ii = ii + 1;
end

ep = unique(ep);
ep = sort(ep);

subs = [];
ii = 1;
jj = 1;
while ii < size(ep,1)
    subs(ii,:) = [ep(jj) ep(jj+1)];
    ii = ii + 1;
    jj = jj + 1;
end

vol = 0.;
for i = 1:size(subs,1)
    vol = vol + vol1(subs(i,:));
end
assert(abs(vol - vol1(o0)) < 1e-12);

%%

function b = subset( I0, I1 )
%SUBSET Returns 1 if I0 is a subset of I2
%   Detailed explanation goes here

b = inside(I1,I0(1)) && inside(I1,I0(2));
