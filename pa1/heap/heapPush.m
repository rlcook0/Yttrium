function heap = heapPush(heap,node)
%Add an element to the heap.
%  HEAP = heapPush(HEAP,NODE) adds element NODE to the HEAP. NODE must
%  contain a member "value" for the heap to heapify by.

    % When a cell array of nodes is specified, push them all onto the heap.
    if numel(node) > 1
        if iscell(node)
            for n=node
                heap = heapPush(heap,n);
            end
        else
            obj.value = 1;
            for i=node
                obj.value = i;
                heap = heapPush(heap,obj);
            end
        end
        return;
    end
    
    if ~isfield(node,'value')
        error('Input node to heapPush must have member "value".');
    end
    
    % Place object at lowest position.
    k = heap.n + 1;
    heap.objects{k} = node;
    heap.n = k;
    
    % Sift inserted element upwards
    kparent = floor(k/2);
    while kparent >= 1
        if heap.objects{kparent}.value > heap.objects{k}.value
            % Swap elements.
            temp = heap.objects{kparent};
            heap.objects{kparent} = heap.objects{k};
            heap.objects{k} = temp;
            k = kparent;
            kparent = floor(k/2);
        else
            break;
        end
    end
end

