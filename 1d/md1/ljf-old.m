function F = ljf( A, rcut, eps, sigma, weight )
%LJF Force (LJ potential)
%   Detailed explanation goes here

F = zeros(size(A));

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

%        assert( abs(r) > 1e-7 );

        w    = 0.5*(weight(i) + weight(j));
        s    = (sigma/r)^6;
        F(i) = F(i) + w*24*eps*s*(1 - 2*s)/r;
        
        j = j + 1;
    end

    j = i-1;
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
       
%        assert( abs(r) > 1e-7 );
        
        w    = 0.5*(weight(i) + weight(j));
        s    = (sigma/r)^6;
        F(i) = F(i) + w*24*eps*s*(1 - 2*s)/r;
        
        j = j - 1;
    end
end
