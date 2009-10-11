function [path,cost,nodesExpanded] = AstarSearch(World)
%Does A* search.
%  path = AstarSearch(World).

% *** YOU MUST IMPLEMENT THIS SEARCH YOURSELF ***
% The lines below create a path that tries to go directly from the starting
% configuration to the goal, but such a path is likely invalid (as these
% two points are not likely to be visible from each other). Delete these
% lines and implement a full, general search.
path = [1 size(World.Landmarks,2)];
cost = norm(World.Landmarks(:,1) - World.Landmarks(:,end));
nodesExpanded = 0;

