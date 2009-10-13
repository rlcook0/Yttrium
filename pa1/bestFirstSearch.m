function [path,cost,nodesExpanded] = bestFirstSearch(World)
%Does best first search.
%  path = bestFirstSearch(World).

% *** YOU MUST IMPLEMENT THIS SEARCH YOURSELF ***
% The lines below create a path that tries to go directly from the starting
% configuration to the goal, but such a path is likely invalid (as these
% two points are not likely to be visible from each other). Delete these
% lines and implement a full, general search.

%[path, cost, nodesExpanded] = AstarSearch(World);

heap = heapCreate();

% List to capture all the nodes we have expanded so far.
expanded = [];
nodesExpanded = 0;
costThusFar = 0;

% Put the first node in the queue/heap.
first_node.value = 0;
first_node.path = [1];
% Needed to keep track of the path's overall cost.
first_node.distanceFromPrev = 0;
heap = heapPush(heap, first_node);

while ~heapIsEmpty(heap)
    [curr, heap] = heapPop(heap);
    
    % Grab the last node in the path.
    curr_index = curr.path(length(curr.path));
    costThusFar = costThusFar + curr.distanceFromPrev;
    
    
    % Check if the last node in the path is a goal.
    if (curr_index == size(World.Landmarks, 2))
        path = curr.path;
        cost = costThusFar;
        
%         fprintf('ANSWER:');
%         for node_num = 1:length(path)
%             coord = World.Landmarks(:,path(node_num));
%             fprintf('%d-(%f, %f) ', curr.path(node_num), coord(1), coord(2));
%         end
%         fprintf('\n');
        
        return;
    end
    
    % Since we haven't...
    nodesExpanded = nodesExpanded + 1;
        
    % Track this node as expanded
    expanded = [expanded curr_index];
    
    % Loop over all successors of the current node.
    successors = find(World.Connectivity(:,curr_index));
    for successor_num = 1:size(successors)
        successor = successors(successor_num);
        
        % Check if we have already expanded this node.
        if (size(find(expanded == successor, 1), 2) ~= 0)    
            continue;
        end
        
        % Make a new node.
        distance = sqrt(sum((World.Landmarks(:,end) - World.Landmarks(:,successor)) .^ 2));
        new_node.value = distance;
        new_node.path = [curr.path successor];
        new_node.distanceFromPrev = sqrt(sum((World.Landmarks(:,curr_index) - World.Landmarks(:,successor)) .^ 2))
        heap = heapPush(heap, new_node);
        
        % TESTING - REMOVE ME
%         for node_num = 1:length(new_node.path)
%             coord = World.Landmarks(:,new_node.path(node_num));
%             fprintf('%d-(%f, %f) ', new_node.path(node_num), coord(1), coord(2));
%         end
%         fprintf('\n');
    end
end

% e.g. return FAIL
path = [];
cost = inf;
