function heapDraw(heap)
%Draw a heap graphically.
%  heapDraw(HEAP). You should have cleared a figure or some axes before
%  calling this function.

hold on;
heapP = heap;
% Get maximum baseline spread
spac = 0.75;
maxy = nextpow2(heap.n+1)-1;
maxBase = -spac/2*((2^maxy)-1) - spac/2;
% Set x,y coordinates of all heap points.
for i=1:heapP.n
    y = nextpow2(i+1)-1;
    heapP.objects{i}.y = y;
%     base = -spac/2*((2^y)-1);
    base = maxBase + spac/2 * 2^(maxy-y);
    heapP.objects{i}.x = base + spac * 2^(maxy-y) * (i-2^y);
end
% Draw splines from children to parent
for i=2:heapP.n
    parenti = floor(i/2);
    plot([heapP.objects{i}.x heapP.objects{parenti}.x],...
         [heapP.objects{i}.y heapP.objects{parenti}.y],'k','LineWidth',2);
end
% Draw spheres and numbers
n = 12;
angs = 2*pi * (0:n-1) / n;
pts = 0.3*[cos(angs);sin(angs)];
for i=1:heapP.n
    fill(heapP.objects{i}.x + pts(1,:),...
         heapP.objects{i}.y + pts(2,:),'c');
    text(heapP.objects{i}.x,heapP.objects{i}.y,...
        sprintf('%d',heapP.objects{i}.value),...
        'HorizontalAlignment','Center',...
        'VerticalAlignment','Middle');
end

