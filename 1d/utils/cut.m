function [ C ] = cut( A, B )
%CUT Compute cut of A and B
%   Detailed explanation goes here

C = [ max(A(1),B(1)) min(A(2),B(2)) ];

% If we do not have an intersection
if C(1) >= C(2)
    C = [C(2) C(2)];
end
