function [  ] = xbbd(  )
%XBBD Xiao-Belytschko Bridging domain method
%   Detailed explanation goes here

% timestep size and maximal number of steps
tau     = 0.05;
nsteps  = 1000;

% the md lattice
req     = 2^(1/6);
A       = req*(-800:100);

% the fe mesh
h       = 5*req;
c4n     = h*(0:500);

% the bridging domain
global bd;
bd      = [ min(c4n)-0.05*req max(A)+0.05*req ];

% the md data
sigma   = 1;
eps     = 1;
rcut    = 1.5*req;
mass    = 1.;
k       = 624*eps*sigma^12/(req^14) - 168*eps*sigma^6/(req^8);
md.u    = zeros(size(A));
md.v    = zeros(size(A));
md.M    = mass*diag(LinearWeight(A));

% the fe data
rho     = @(x) ones(size(x))*numel(A)/(A(numel(A)) - A(1));
fe.M    = lump( femass(c4n, @ComplLinearWeight, rho) );
fe.d    = zeros(size(c4n));
fe.v    = zeros(size(c4n));

% find nodes, node indices, atoms and atom indices for the hanshake
% region
hs.c4n  = [];
hs.nix  = [];
hs.A    = [];
hs.aix  = [];

ij = 1;
for i = 1:numel(A)
    if inside(bd, A(i))
        hs.A(ij)   = A(i);
        hs.aix(ij) = i;
        ij         = ij + 1;
    end
end
ij = 1;
for i = 1:numel(c4n)
    if inside(bd, c4n(i))
        hs.c4n(ij) = c4n(i);
        hs.nix(ij) = i;
        ij         = ij + 1;
    end
end

% compute interpolation matrix
N       = interpol(hs.A, hs.c4n);

% mass matrix for the constraints
cs.feM  = lump( femass(c4n, @ComplLinearWeight, rho) );
cs.feM  = cs.feM(hs.nix,hs.nix);
cs.mdM  = mass*diag(LinearWeight(hs.A));

% compute multiplier matrix
L       = lump( N*inv(cs.feM)*N' + inv(cs.mdM) );

% initial amplitude
md.u    = initialAmplitude(A, req);

% figure for displacements
figs.d  = figure;
% figure for energy
figs.e  = figure;

% initialize forces
md.F    = ljf(A + md.u, rcut, eps, sigma, LinearWeight(A));
fe.F    = fecybnf(c4n, fe.d, sigma, eps, req, ComplLinearWeight(c4n));

% range of important atoms
rg = ( find(A >= -200*req, 1):size(A,2) );

% energy computation routines
EnergyMD  = @(u,v) 0.5*sum(v(rg)*md.M(rg,rg)*v(rg)') ...
                    + lje(A(rg) + u(rg), rcut, eps, sigma, LinearWeight(A(rg)));
EnergyFE  = @(u,v) 0.5*sum(v*fe.M*v') ... 
                    + fecybne(c4n, u, sigma, eps, req, @ComplLinearWeight);
             
% reference energy
energyMD0 = EnergyMD( zeros(size(md.u)), zeros(size(md.v)) );
energyFE0 = EnergyFE( zeros(size(fe.d)), zeros(size(fe.v)) );

for i = 1:nsteps

    md.v = md.v + 0.5*tau*( md.M \ md.F' )';
    fe.v = fe.v + 0.5*tau*( fe.M \ fe.F' )';
    
    md.u = md.u +     tau*md.v;
    fe.d = fe.d +     tau*fe.v;
    
    % compute new forces
    md.F = ljf(A + md.u, rcut, eps, sigma, LinearWeight(A));
    fe.F = fecybnf(c4n, fe.d, sigma, eps, req, ComplLinearWeight(c4n));
    
    md.v = md.v + 0.5*tau*( md.M \ md.F' )';
    fe.v = fe.v + 0.5*tau*( fe.M \ fe.F' )';
    
    % compute lagrange multiplier and correct
    mu           = L \ ( md.v(hs.aix) - (N*fe.v(hs.nix)')' )';
    md.v(hs.aix) = md.v(hs.aix) - ( cs.mdM \ mu )';
    fe.v(hs.nix) = fe.v(hs.nix) + ( cs.feM \ (N'*mu) )';
    
%    % Check residual
%    assert( norm(md.v(hs.aix) - (N*fe.v(hs.nix)')') < 1e-12 );
    
    % plot displacements
    if 0 == mod(i,10)
        figure(figs.d);
        
        lh = plot(c4n, fe.d, 'o-b', A(rg), md.u(rg), '-r');
        set(lh, 'LineWidth', 2);
        
        xlim([min(A(rg))-0.5*req 300+0.5*req]);
        ylim([-0.002 0.01]);
                
        % savefig()?
    end
    
    % plot energy
    if 0 == mod(i,50) && i > 99
        if 100 == i            
            % reference energy
            energyDenom = EnergyMD(md.u, md.v) - energyMD0 ...
                        + EnergyFE(fe.d, fe.v) - energyFE0;
        end
        
        NormalizedEnergyMD = @(E) (E - energyMD0*ones(size(E)))/energyDenom;
        NormalizedEnergyFE = @(E) (E - energyFE0*ones(size(E)))/energyDenom;
        
        md.E(i/50-1) = EnergyMD(md.u, md.v);
        fe.E(i/50-1) = EnergyFE(fe.d, fe.v);
        
        figure(figs.e);
                
        lh = plot(tau*(1:i/50-1), NormalizedEnergyMD(md.E), 'r-', ...
                  tau*(1:i/50-1), NormalizedEnergyFE(fe.E), 'b-');
        set(lh, 'LineWidth', 2);
        
        ylim([ 0  1 ]);
                
        % savefig()?
    end
end

end

%%

function [ d ] = initialAmplitude(P, req)
%INITIALAMPLITUDE The typical wave
%   Detailed explanation goes here

M     = -200*req*ones(size(P));
A     = 0.015;
sigma = 30.; % 15.;
b     = 0.1;
Lc    = 5*sigma;
uc    = exp(-(Lc/sigma)^2);
H     = sigma/4;

d     = A*(( A*exp( - ((P - M)/sigma).^2) - uc)/(A - uc) ) ...
        .* ( 1 + b*cos((2*pi*(P - M))/H));

end

%%

function [ val ] = LinearWeight( x )
%LinearWeight linear weighting function, is 1 on the MD side
global bd;

% Lower treshold for weighting and therefor for the masses
LOWER_TRESHOLD = 0.001;

val = ones(size(x));
for i = 1:numel(x)
        if x(i) < bd(1)
                val(i) = 1.;
        elseif x(i) > bd(2)
                val(i) = 0.;
        else
                val(i) = 1. - (x(i) - bd(1))/vol1(bd);
        end
end

val = max(val,LOWER_TRESHOLD);

end

%%

function [ val ] = ComplLinearWeight( x )
%ComplLinearWeight Complement of linear weighting function
global bd;

% Lower treshold for weighting and therefor for the masses
LOWER_TRESHOLD = 0.001;

val = ones(size(x));
for i = 1:numel(x)
        if x(i) < bd(1)
                val(i) = 0.;
        elseif x(i) > bd(2)
                val(i) = 1.;
        else
                val(i) = (x(i) - bd(1))/vol1(bd);
        end
end

val = max(val,LOWER_TRESHOLD);

end
