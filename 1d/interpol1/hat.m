function val = hat( o, p, x )
%FESHAPE Evaluate hat function on PATCH o of
%        node p at global coordinates x
%   Detailed explanation goes here

% WARNING: o must be the patch of some node

% Is the node a boundary node?
isbdy = abs(o(1) - p) < 1e-12 || abs(o(2) - p) < 1e-12;

% Transformation matrix 
if isbdy
    J = vol1(o);
else
    J = 0.5*vol1(o);
end

val = max(0,1. - abs((x - p)/J));
