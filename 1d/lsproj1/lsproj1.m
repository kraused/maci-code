function [P] = lsproj1(atoms, nodes)
%LSPROJ1 Summary of this function goes here
%   Detailed explanation goes here

N = interpol(atoms, nodes);
P = inv(N'*N)*N';
