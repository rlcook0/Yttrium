%% CS221  Programming Assignment #1  Heap Example

heap = heapCreate(); % Create a min heap.

% Add elements 15 through 1, drawing heap, and pausing between each.
disp('Adding elements 15 through 11.');
for i=15:-1:1
    % Create object with "value" field instantiated.
    obj.value = i;
    heap = heapPush(heap,obj);
    figure(1), clf, axis equal, axis ij;
    heapDraw(heap);
    title(sprintf('Added %d through %d',1,i));
    pause(0.5);
%     pause;
end

%% Remove elements till heap empty
disp('Emptying heap.');
while ~heapIsEmpty(heap)
    [top,heap] = heapPop(heap);
    figure(1), clf, axis equal, axis ij;
    heapDraw(heap);
    title(sprintf('Added %d through %d',1,i));
    pause(0.5);
%     pause;
end

%% Add elements in random order
% heap = heapCreate('max');
T = randperm(15);
% Add elements 1 through 15, drawing heap, and pausing between each.
disp('Adding elements in this order:');
T
for i=1:length(T)
    % Create object with "value" field instantiated.
    obj.value = T(i);
    heap = heapPush(heap,obj);
    figure(1), clf, axis equal, axis ij;
    heapDraw(heap);
    title(sprintf('Added first %d elements',i));
    pause(0.5);
%     pause;
end

