function V = lje( A, rcut, eps, sigma, weight )
%LJE Energy (LJ potential)
%   Detailed explanation goes here

V = 0.;

for i = 1:size(A,2)
    j = i+1;
    while 1 == 1
        if j < 1
            break;
        end
        if j > size(A,2)
            break;
        end
        
        r = A(j) - A(i);

        if abs(r) > rcut
            break;
        end

        assert( abs(r) > 1e-7 );

        s = (sigma/r)^6;
        V = V + weight(i)*4*eps*s*(s-1);

        j = j + 1;
    end

%    j = i-1;
%    while 1 == 1
%        if j < 1
%            break;
%        end
%        if j > size(A,2)
%            break;
%        end
%        
%        r = A(j) - A(i);
%
%        if abs(r) > rcut
%            break;
%        end
%       
%        assert( abs(r) > 1e-7 );
%        
%        s = (sigma/r)^6;
%        V = V + weight(i)*4*eps*s*(s-1);           
%
%        j = j - 1;
%    end
end

% Remember that we compute the sum over the whole
% indexset not only over the upperdiagonal index
% tuples.
%V = 0.5*V;

