function dF = pml( M, D, u, v )
%PML Summary of this function goes here
%   Detailed explanation goes here

dF = -(D*(D*u' + 2*v'))';
dF = (M*dF')';
