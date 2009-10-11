function Field = getAndDrawPotentialField(World)
%Get and plot potential field values.
%  Field = getAndDrawPotentialField(World) returns Field with
%  members Values, Xs, and Ys. Values is a length(Ys)-by-length(Xs) matrix
%  which stores the potential field value at each location.

figure(1), clf, hold on, grid on, axis equal;
delta = 0.05;
xs = World.axis(1):delta:World.axis(2);
ys = World.axis(3):delta:World.axis(4);
Values = zeros(length(ys),length(xs));
for i=1:length(ys)
    for j=1:length(xs)
        Values(i,j) = getPotentialValue(World,[xs(j) ys(i)]);
    end
end
% Draw potential values with jet colormap
imagesc(Values), axis image, colormap jet;
xt = get(gca,'XTick');
yt = get(gca,'YTick');
set(gca,'XTick',xt,'XTickLabel',xs(xt));
set(gca,'YTick',yt,'YTickLabel',ys(yt));
Field.Values = Values;
Field.Xs = xs;
Field.Ys = ys;

