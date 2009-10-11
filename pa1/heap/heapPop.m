function [top,heap,value] = heapPop(heap,n)
%Pop top element from the heap.
%  [TOP,HEAP,VALUE] = heapPop(HEAP,N=1) pops N elements from the heap, by
%  default 1. Returns the TOP element, the HEAP with the top element
%  removed, and the VALUE of the TOP element.

    if nargin >= 2 && ~isempty(n) && n > 1
        top = cell(1,n);
        value = zeros(1,n);
        for i=1:n
            [top{i},heap,value(i)] = heapPop(heap);
        end
        return;
    end

    top = heap.objects{1};
    value = heap.objects{1}.value;
    % Swap last element up to top position
    heap.objects{1} = heap.objects{heap.n};
    heap.n = heap.n-1; % Decrement size
    
    % Sift promoted element downwards
    k = 1;
    while 2*k <= heap.n    
        % Loop while there is at least one child to look at
        v = heap.objects{k}.value;
        lv = heap.objects{2*k}.value;
        rv = inf;
        if 2*k+1 <= heap.n
            rv = heap.objects{2*k+1}.value;
        end
        % Compare with both children values
        if lv < rv
            if lv < v
                % Compare with left child
                temp = heap.objects{2*k};
                heap.objects{2*k} = heap.objects{k};
                heap.objects{k} = temp;
                k = 2*k;
            elseif rv < v
                % Compare with right child
                temp = heap.objects{2*k+1};
                heap.objects{2*k+1} = heap.objects{k};
                heap.objects{k} = temp;
                k = 2*k+1;
            else
                break;
            end
        elseif rv < v
            % Compare with right child
            temp = heap.objects{2*k+1};
            heap.objects{2*k+1} = heap.objects{k};
            heap.objects{k} = temp;
            k = 2*k+1;
        else
            break;
        end
    end
end


