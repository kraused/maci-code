function [P M R] = l2proj1(h, atoms, nodes, rho)
%L2PROJ1 Summary of this function goes here
%   Detailed explanation goes here

% --------------------
% Compute overlapped domain
% --------------------
D = [min(atoms)-h max(atoms)+h];

% --------------------
% Build matrix M
% --------------------
M = sparse(size(nodes,2),size(nodes,2));

for i = 1:size(nodes,2)-1
    T = [nodes(i) nodes(i+1)];
    S = cut(T,D);
    
    if vol1(S) < 1e-12
        continue
    end
    
    idx    = [i i+1];
    locmat = quad_shape_x_mult(T,S,rho);
    
    for j = 1:2
        for k = 1:2
            M(idx(j),idx(k)) = M(idx(j),idx(k)) + locmat(j,k);
        end
    end   
end

% --------------------
% Build matrix R
% --------------------
R = sparse(size(nodes,2),size(atoms,2));

for i = 1:size(nodes,2)-1
    for j = 1:size(atoms,2)
        p = atoms(j);
        
        T = [nodes(i) nodes(i+1)];
        S = cut(T,pupatch(p,h));
        
        if vol1(S) < 1e-12
            continue
        end
        
        idx  = [i; i+1];
        locr = quad_mult_x_pu(T,S,atoms,j,h,rho);
        
        for k = 1:2
            R(idx(k),j) = R(idx(k),j) + locr(k);
        end
    end
end

P   = inv(M)*R;

%%

function M = quad_shape_x_mult(E,S,rho)
%QUAD_SHAPE_X_MULT quadrature on element for assembling of shape x multiplier
%                  matrix

J       = vol1(E);
% Transformation to the reference element of E
E2Ref   = @(x) (x - E(1))/J;
% Shapefunctions on the reference element
lambda  = { ( @(x) (1. - x) ) ( @(x) x ) };

K       = vol1(S);
% Transformation from the reference element to S
Ref2S   = @(x) K*x + S(1);

% Use the simpson rule
quadp   = [ 0 0.5 1 ];
weights = [ 1/6 4/6 1/6 ];

M = zeros(2,2);
for p = 1:size(quadp,2)
    for i = 1:2
        for j = 1:2
            M(i,j) = M(i,j) + K*weights(p)* ...
                     lambda{i}(E2Ref(Ref2S(quadp(p))))*lambda{j}(E2Ref(Ref2S(quadp(p))))* ...
                     rho(Ref2S(quadp(p)));
        end
   end
end

%%

function val = isnonoverlapping(A,h)
%ISNONOVERLAPPING Check if particle covering is nonoverlapping
%   Returns 1 if the covering is nonoverlapping
mindist = abs(A(size(A,2)) - A(1));
maxdist = 0;
for i = 2:size(A,2)
	mindist = min(mindist,abs(A(i) - A(i-1)));
	maxdist = max(maxdist,abs(A(i) - A(i-1)));
end

% Tolerance
EPS = 1e-12;

if maxdist - mindist < EPS && abs(2*h - mindist) < EPS
	val = 1;
else
	val = 0;
end

%%

function val = shepard(k,h,x,A)
%SHEPARD Evaluate shepard
%   Evaluate the shepard on patch [p-h,p+h] in x

% Handle nonoverlapping case
if 1 == isnonoverlapping(A,h)
	val = 1.;
else
	J     = 2*h;
	nom   = spline1((x - (A(k)-h))/J);
	denom = 0.;

	for i = 1:size(A,2)
		denom = denom + spline1((x - (A(i)-h))/J);
	end

	if denom < 1e-12
		assert(nom < 1e-12);
	end

	val = nom/denom;	
	assert(0 == isnan(val));
end


%%

function val = spline1(x)
%SPLINE1 Evaluate on reference element [0 1]
 
if x < 0 || x > 1.
    val = 0.;
elseif x < 0.5
    val = x;
else
    val = 1 - x;
end

%%

function R = quad_mult_x_pu(E,S,atoms,k,h,rho)
%QUAD_MULT_X_PU quadrature for the assembling of the scale transfer matrix
%               R

[trash0 trash1 subs] = ConstructSubs(atoms,k,h);

J      = vol1(E);
% Transformation to the reference element from E
E2Ref  = @(x) (x - E(1))/J;
% Shapefunctions on the reference element
lambda = { ( @(x) (1. - x) ) ( @(x) x ) };

% Use the simpson rule
quadp   = [ 0 0.5 1 ];
weights = [ 1/6 4/6 1/6 ];

R = [0. 0.];
for i = 1:size(subs,1)
    % Quadrature on C = subs(i,:) cap S
    C = cut(S,subs(i,:));
    K = vol1(C);
    if K < 1e-12
        continue
    end
    
    % Transformation from the ref element to C
    Ref2C = @(x) K*x + C(1);
    
    % Evaluation points in C
    evp = Ref2C(quadp);
    
    for p = 1:size(quadp,2)
        for j = 1:2
            R(j) = R(j) + K*weights(p)*lambda{j}(E2Ref(evp(p)))*shepard(k,h,evp(p),atoms)*rho(evp(p));
        end
    end
end
