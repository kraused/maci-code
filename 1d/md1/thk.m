function u0 = thk( id, u, m, k, tau, refined, varargin )
%THK Computes the displacement of the neighbouring
%    atom for nearest neighbour MD simulations with
%    sharp boundary
%   Detailed explanation goes here

global o;

% k must be the stiffness
o = sqrt(k/m);

persistent x;

% Maximal number of boundary atoms
MAX_ID = 2;

if isempty(x)
    for i = 1:MAX_ID
        x(i).step   = 0;
        x(i).u(1)   = u;
        x(i).uninit = 1;
    end
end
if x(id).uninit == 1
    x(id).u(1)   = u;
    x(id).uninit = 0;
    return;
end

x(id).step            = x(id).step + 1;
x(id).u(x(id).step+1) = u;

% cutoff/truncation control
if isempty(varargin)
    cutoff = 200;
else
    cutoff = cell2mat(varargin(1));
end

% time steps
ti = [max(0,x(id).step-cutoff) x(id).step];

% relevant displacement
u  = x(id).u(ti(1)+1:ti(2)+1);

% Compute impedance force
if 1 == refined
	u0 = thk_convolution_refined_trapez(tau*ti, tau, u);
else
	u0 = thk_convolution_summed_trapez(tau*ti, tau, u);
end

%%

function u0 = thk_convolution_summed_trapez(I, tau, u)
%THK_CONVOLUTION_SUMMED_TRAPEZ compute convolution integral using
%                              summed trapez rule

% eigen frequency
global o;

% length of integration interval
T = vol1(I);

% number of intervalls
n  = int32(T/tau);
assert(n+1 == size(u,2));

% THK
theta = @(t) 2*besselj(2,2.*o*t)./t;

% Right interval boundary; left
% one is irrelevant
u0 = 0.5*theta(T)*u(1);

for i = 2:n
    u0 = u0 + theta(T - double(i-1)*tau)*u(i);
end

u0 = tau*u0;

%%

function u0 = thk_convolution_refined_trapez(I, tau, u)
%THK_CONVOLUTION_REFINED_TRAPEZ compute convolution integral using summed
%                              trapez rule and linear interpolation for the
%                              displacement

% eigen frequency
global o;

% THK
theta = @(t) 2*besselj(2,2.*o*t)./t;

% Fine timescale for integration
TAU  = tau/2;

% length of integration interval
T = vol1(I);

% Original mesh
x0 = (I(1):tau:I(2)); 
% New mesh
x1 = (I(1):TAU:I(2));

% Interpolate u
ui = interp1(x0,u,x1);

n  = int32(T/TAU);
assert(n+1 == size(ui,2));

% Right interval boundary; left
% one is irrelevant
u0 = 0.5*theta(T)*ui(1);

for i = 2:n
    u0 = u0 + theta(T - double(i-1)*TAU)*ui(i);
end

u0 = TAU*u0;

