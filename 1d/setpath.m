
F = { 'utils', 'md1', 'fe1', 'interpol1', 'lsproj1', 'l2proj1' };

for i = 1:numel(F)
    path(path, sprintf('%s/%s', pwd, F{i}));
end
