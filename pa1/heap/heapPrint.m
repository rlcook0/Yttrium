function heapPrint(heap)
%Print heap contents.
%  heapPrint(HEAP) prints the contents of the heap.

    % Loop through elements
    fprintf('Heap contents:\n');
    if heap.n == 0
        fprintf('-- Heap empty --\n');
        return;
    end
    % Print root
    fprintf('Root - %d\n', heap.objects{1}.value);
    % Print remaining elements
    for i=2:heap.n
        fprintf('Node %s - %d\n', getNodePathStr(i), heap.objects{i}.value);
    end
end

function str = getNodePathStr(i)
    k = nextpow2(i+1)-1; % Get length of string.
    str = repmat(' ',[1 k]);
    while i > 1
        if mod(i,2) == 0
            str(k) = 'L';
        else
            str(k) = 'R';
        end
        i = floor(i/2);
        k = k-1;
    end
end

