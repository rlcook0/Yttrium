function [path,cost,nodesExpanded] = uniformCostSearch(World)

heap = heapCreate();

% List to capture all the nodes we have expanded so far.
expanded = [];
nodesExpanded = 0;

% Put the first node in the queue/heap.
first_node.value = 0;
first_node.path = [1];
heap = heapPush(heap, first_node);

while ~heapIsEmpty(heap)
    [curr, heap] = heapPop(heap);
    
    % Grab the last node in the path.
    curr_index = curr.path(length(curr.path));
    
    % Check if the last node in the path is a goal.
    if (curr_index == size(World.Landmarks, 2))
        path = curr.path;
        cost = curr.value;
        return;
    end
    
    % Check if we have already expanded this node.
    if (size(find(expanded == curr_index, 1), 2) ~= 0)    
        continue;
    end
    
    % Since we haven't...
    nodesExpanded = nodesExpanded + 1;
    expanded = [expanded curr_index];
    
    % Loop over all successors of the current node.
    successors = find(World.Connectivity(:,curr_index));
    for successor_num = 1:size(successors)
        successor_index = successors(successor_num);

        % Make a new node.
        distance = sqrt(sum((World.Landmarks(:,curr_index) - World.Landmarks(:,successor_index)) .^ 2));
        
        new_node.value = curr.value + distance;
        new_node.path = [curr.path successor_index];
        
        heap = heapPush(heap, new_node);
    end
end

% e.g. return FAIL
path = [];
cost = inf;
