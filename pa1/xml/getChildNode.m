function node = getChildNode(parent,name)
%Get a child node with a given name.
%  NODE = getChildNode(PARENT,NAME) returns the child node of PARENT that
%  has a field "Name" value of NAME.
%
%  Copyright (c) 2009, Stanford University
%
%  AUTHOR(S): Paul Baumstarck <pbaumstarck@stanford.edu>

node = [];
n = length(parent.children);
for i=1:n
    if strcmp(parent.children{i}.Name,name)
        node = parent.children{i};
        break;
    end
end
