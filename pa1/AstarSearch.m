function [path,cost,nodesExpanded] = AstarSearch(World)
%Does A* search.
%  path = AstarSearch(World).

% *** YOU MUST IMPLEMENT THIS SEARCH YOURSELF ***
% The lines below create a path that tries to go directly from the starting
% configuration to the goal, but such a path is likely invalid (as these
% two points are not likely to be visible from each other). Delete these
% lines and implement a full, general search.

heap = heapCreate();

expanded = [];

first_node.value = 0;
first_node.sofar = 0;
first_node.path = [1];
heap = heapPush(heap, first_node);

nodesExpanded = 0;

while ~heapIsEmpty(heap)
    [top, heap] = heapPop(heap);
        
    last = top.path(length(top.path));
    if (last == size(World.Landmarks, 2))
        path = top.path
        cost = top.sofar;
        x = 2
        return;
    end
    
    successors = find(World.Connectivity(:,last));
    for suc_num = 1:size(successors)
        suc = successors(suc_num);

        find(expanded == suc, 1);
        size(find(expanded == suc, 1), 2);
        
        if (size(find(expanded == suc, 1), 2) ~= 0)    
            continue;
        end
        
        expanded = [expanded suc];
        nodesExpanded = nodesExpanded + 1;
        
        World.Landmarks(:,last);
        World.Landmarks(:,suc);
        dFrom = sqrt(sum((World.Landmarks(:,last) - World.Landmarks(:,suc)) .^ 2));
        dTo   = sqrt(sum((World.Landmarks(:,size(World.Landmarks, 2)) - World.Landmarks(:,suc)) .^ 2));
        new_node.value = top.sofar + dFrom + dTo;
        new_node.sofar = top.sofar + dFrom;
        new_node.path = [top.path suc];
        heap = heapPush(heap, new_node);
    end
end

x = 1

path = [];
cost = inf;

% path = [1 size(World.Landmarks,2)];
% cost = norm(World.Landmarks(:,1) - World.Landmarks(:,end));
% nodesExpanded = 0;

