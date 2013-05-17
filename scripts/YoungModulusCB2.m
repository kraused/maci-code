
%
% YoungModulusCB2: Computes nu, e, mu, lambda for the 
%                  Cauchy-Born material law in two-dimensions
%
% The script takes no arguments
%

Cxx  = @(A) A(1,1);
Cxy  = @(A) A(1,2);
Cyx  = @(A) A(2,1);
Cyy  = @(A) A(2,2);

r0   = 2^(1/6);
l1   = r0 * [    1,         0 ];
l2   = r0 * [  1/2, sqrt(3)/2 ];
l3   = r0 * [ -1/2, sqrt(3)/2 ];
th   = sqrt(3)*r0^2/4;

%Eps = @(F) 0.5*(F'*F - [1,0;0,1]);
Eps = @(F) 0.5*(F+F'-[2,0;0,2]);

% Harmonic potential
%Pot  = @(x) 0.5*(norm(x)-r0)^2;
%Frc  = @(x) (norm(x)-r0)/norm(x)*x;

% LJ potential
eps   = 1;
sigma = 1;
Pot   = @(x)  4*eps*((sigma/norm(x))^12 -   (sigma/norm(x))^6 );
Frc   = @(x) 24*eps*((sigma/norm(x))^6  - 2*(sigma/norm(x))^12)/(x'*x)*x;

W = @(F) 1./(2*th)*(Pot(F*l1')    + Pot(F*l2')    + Pot(F*l3')   );
S = @(F) 1./(2*th)*(Frc(F*l1')*l1 + Frc(F*l2')*l2 + Frc(F*l3')*l3);

% Here is what I did to derive these numbers:
% I want to have the form S = lambda*trace(eps) + 2*mu*eps, looking at the
% form of these terms I can see that lambda = E-2*mu
% To derive E i used the stress-stain relation for F = [1+dx,0;0,1]

% Linear elasticity
lambda = 24.66;
mu     = 24.7451;

%E  = 66.0176*2/3;
%nu = 1/3;

%disp([ ' exp E  = ', num2str(mu*(3*lambda + 2*mu)/(lambda + mu)) ]);
%disp([ ' exp nu = ', num2str(lambda/(2*(lambda + mu))) ]);

S  = @(F) lambda*trace(Eps(F))*[1,0;0,1] + 2*mu*Eps(F);
%S    = @(F) E/((1+nu)*(1-2*nu))*[ (1-  nu)*Cxx(Eps(F)) + nu*Cxy(Eps(F)), ...
%                                  (1-2*nu)*Cxy(Eps(F)); ...
%                                  (1-2*nu)*Cxy(Eps(F)), ...
%                                  (1-  nu)*Cyy(Eps(F)) + nu*Cxy(Eps(F)) ];
%S    = @(F) E/(1-nu^2)*[ Cxx(Eps(F)) + nu*Cxy(Eps(F)), ...
%                         (1-nu)*Cxy(Eps(F)); ...
%                         (1-nu)*Cxy(Eps(F)), ...
%                         Cyy(Eps(F)) + nu*Cxy(Eps(F)) ];

F = [ 1, 0; 0, 1 ];

e = [];
s = [];

i = 1;
for dx = 0.001:0.001:0.01
    e(i) = dx;
    
    X    = @(z) norm(S([1+z(1),0;0,1-z(2)])-[dx,0;0,0],'fro');
    opts = optimset('Display', 'off', 'Diagnostics', 'off', ...
                    'Algorithm', 'levenberg-marquardt', 'TolFun', 1e-8, ...
                    'MaxIter', 1000);
    z    = fsolve(X, [0,0], opts);

%    F = [ 1+dx, 0; 0, 1];
%    E = Eps(F);
%    P = S(F);
%    s(i) = P(1,1)/E(1,1);    
    
    s(i) = dx/z(1);
    t(i) = z(2)/z(1);
    i    = i+1;
end

x  = polyfit(e, t, 1);
nu = x(2);
disp([' guess for nu     = ', num2str(nu) ]);

x = polyfit(e, s, 1);
Y = x(2);
disp([' guess for E      = ', num2str(Y) ]);

F = [ 1, 0; 0, 1 ];

e = [];
s = [];

i = 1;
for dx = 0.00001:0.00001:0.001
    F(1,2) = dx;
    
    E      = Eps(F);   
    e(i)   = norm(E,'fro');
    
    P      = S(F);
    s(i)   = P(1,2)/(2*E(1,2));
    
    i      = i+1;
end

x  = polyfit(e, s, 1);
mu = x(2);
disp([' guess for mu     = ', num2str(mu) ]);

lambda = mu*(Y-2*mu)/(3*mu-Y);
disp([' guess for lambda = ', num2str(lambda) ]);

%F = [ 1, 0; 0, 1 ];
%
%e = [];
%s = [];
%
%i = 1;
%for dx = 0.00001:0.00001:0.001
%    F(1,1) = 1+dx;
%    
%    E      = Eps(F);
%    e(i)   = E(1,1);
%    
%    X    = @(z) Cyy(S([ F(1,1), 0; 0, z ]));
%    opts = optimset('Display', 'off');
%    z    = fsolve(X, 1., opts);    
%    s(i) = -0.5*(z*z-1)/E(1,1);
%    
%    i      = i+1;
%end
%
%x  = polyfit(e, s, 1);
%nu = x(2);
%disp([' guess for nu     = ', num2str(nu) ]);
