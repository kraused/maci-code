function F = linljf( u, req, eps, sigma, weight )
%LINLJF Linearized LJ force
%   Detailed explanation goes here

k = 624*eps*sigma^12/(req^14) - 168*eps*sigma^6/(req^8);

F = zeros(size(u));

%FIXME: What to do with the weighting?

if 1 == size(u,2)
	F = k*( - 2*u(1) );
    return;
end

r = size(u,2);

% FIXME: Was gehoert hier hin? Variante 1 funktioniert mit
%        dem THK aus dem Lui/Karpov/Park buch fuehrt aber zu
%        falschen Randbedingungen (wenn man es mit einem
%        normalen LJ lauf vergleicht).

%%% F(1) = k*(        - 2*u(1) + u(2) );
%%% F(r) = k*( u(r-1) - 2*u(r)        );

F(1) = k*(u(2  ) - u(1));
F(r) = k*(u(r-1) - u(r));

%%% 
%%% % -----------------
%%% % Periodic boundary
%%% % -----------------
%%% F(1) = k*( u(r  ) - 2*u(1) + u(2) );
%%% F(r) = k*( u(r-1) - 2*u(r) + u(1) );

if 2 < r
    for i = 2:r-1
        F(i) = k*( u(i-1) - 2*u(i) + u(i+1) );
    end
end
