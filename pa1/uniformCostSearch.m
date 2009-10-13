function [path,cost,nodesExpanded] = uniformCostSearch(World)

heap = heapCreate();

% List to capture all the nodes we have expanded so far.
expanded = [];
nodesExpanded = 0;

% Put the first node in the queue/heap.
first_node.value = 1;
first_node.path = [1];
heap = heapPush(heap, first_node);

while ~heapIsEmpty(heap)
    % Grab the last node in the path.
    [top, heap] = heapPop(heap);
    curr = top.path(length(top.path));
    
    % Check if the last node in the path is a goal.
    if (curr == size(World.Landmarks, 2))
        path = top.path;
        cost = top.value;
        return;
    end
    
    % Loop over all successors of the current node.
    successors = find(World.Connectivity(:,curr));
    for successor_num = 1:size(successors)
        successor = successors(successor_num);
        
        % Check if we have already expanded this node.
        if (size(find(expanded == successor, 1), 2) ~= 0)    
            continue;
        end
        
        % Since we haven't...
        nodesExpanded = nodesExpanded + 1;
        
        % Add successor to the path.
        expanded = [expanded successor];
        
        % Make a new node.
        distance = sqrt(sum((World.Landmarks(:,curr) - World.Landmarks(:,successor)) .^ 2));
        new_node.value = top.value + distance;
        new_node.path = [top.path successor];
        heap = heapPush(heap, new_node);
    end
end

% e.g. return FAIL
path = [];
cost = inf;
