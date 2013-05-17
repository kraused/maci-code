function [ ] = make( varargin )
%MAKE Summary of this function goes here
%   Detailed explanation goes here

if 1 == nargin
    if strcmp('clean',varargin{1})
        delete lje.mex*
        delete ljf.mex*
        return
    end
end

mex lje.c
mex ljf.c

end

