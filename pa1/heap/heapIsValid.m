function valid = heapIsValid(heap)
%Return whether the heap is valid or not.
%  VALID = heapIsValid(HEAP).

    valid = true;
    for i=2:heap.n
        parent = floor(i/2);
        if heap.objects{i}.value < heap.objects{parent}.value
            valid = false;
            return;
        end
    end
end

