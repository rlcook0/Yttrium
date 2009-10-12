function [path,cost,nodesExpanded] = uniformCostSearch(World)

heap = heapCreate();

first_node.value = 1;
first_node.path = [1];
heap = heapPush(heap, first_node);

nodesExpanded = 0;

while ~heapIsEmpty(heap)
    10000
    [top, heap] = heapPop(heap);
    last = top.path(length(top.path))
    if (last == size(World.Landmarks, 2))
        path = top.path;
        cost = top.value;
        return;
    end
    
    successors = find(World.Connectivity(:,last));
    for suc_num = 1:size(successors)
        nodesExpanded = nodesExpanded + 1;
        suc = successors(suc_num)
        
        World.Landmarks(:,last);
        World.Landmarks(:,suc);
        distance = sqrt(sum((World.Landmarks(:,last) - World.Landmarks(:,suc)) .^ 2));
        new_node.value = top.value + distance;
        new_node.path = [top.path suc];
        heap = heapPush(heap, new_node);
    end
end

x = 1;
%Does uniform-cost search.
%  path = uniformCostSearch(World).

% *** YOU MUST IMPLEMENT THIS SEARCH YOURSELF ***
% The lines below create a path that tries to go directly from the starting
% configuration to the goal, but such a path is likely invalid (as these
% two points are not likely to be visible from each other). Delete these
% lines and implement a full, general search.
%path = [1 size(World.Landmarks,2)];
%cost = norm(World.Landmarks(:,1) - World.Landmarks(:,end));
%nodesExpanded = 0;

