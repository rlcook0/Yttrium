function ret = isPathValid(World,path)
%Returns if a path is valid or not.
%  RET = isPathValid(WORLD,PATH).

n = size(World.Landmarks,2);
if path(1) ~= 1 || path(end) ~= n
    ret = false;
    return;
end
% Trace connectivity
for i=2:length(path)
    if ~World.Connectivity(path(i),path(i-1)) && ...
        ~World.Connectivity(path(i-1),path(i))
        ret = false;
        return;
    end
end
ret = true;

