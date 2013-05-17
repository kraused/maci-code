function [ boolval ] = inside( E, p )
%INSIDE Returns 1 if p is inside the cell E
%   Detailed explanation goes here

boolval = ( E(1) <= p ) & ( p <= E(2) );