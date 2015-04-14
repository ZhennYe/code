% VER_TREE   Verifies the integrity of a tree.
% (trees package)
% 
% ver_tree (intree)
% -----------------
%
% verifies the integrity of a tree and creates warnings that precede common
% errors. Is called by basically every single TREES package function. Could
% be useful for something else maybe...
%
% Input
% -----
% intree::integer:index of tree in trees or structured tree
%
% Output
% ------
% issues warnings only...
%
% Example
% -------
% sample = sample_tree; sample.X = 0;
% ver_tree (sample);
%
% See also start_trees
% Uses X Y Z D R dA
%
% the TREES toolbox: edit, visualize and analyze neuronal trees
% Copyright (C) 2009  Hermann Cuntz

function ver_tree (intree)

% trees : contains the tree structures in the trees package
global trees

% use full tree for this function
if ~isstruct (intree),
    tree = trees {intree};
else
    tree = intree;
end

if isfield (tree, 'dA'),
    if length (size (tree.dA)) ~= 2,
        warning ('Trees:NoTree', 'adjacency matrix incorrect dimensions');
    else
        if size (tree.dA, 1) ~= size (tree.dA, 2),
            warning ('Trees:NoTree', 'adjacency matrix not square');
        end
    end
else
    warning ('Trees:NoTree', 'missing adjacency matrix');
end

if isfield (tree, 'X'),
    if (size (tree.X, 2) ~= 1) || (length (size (tree.X)) ~= 2),
        warning ('Trees:NoTree', 'X not vertical vector');
        if size (tree.X, 1) ~= size (tree.dA, 1),
            warning ('Trees:NoTree', 'X size not compatible with adjacency matrix');
        end
    end
end

if isfield (tree, 'Y'),
    if (size (tree.Y, 2) ~= 1) || (length (size (tree.Y)) ~= 2),
        warning ('Trees:NoTree', 'Y not vertical vector');
        if size (tree.Y, 1) ~= size (tree.dA, 1),
            warning ('Trees:NoTree', 'Y size not compatible with adjacency matrix');
        end
    end
end

if isfield (tree, 'Z'),
    if (size (tree.Z, 2) ~= 1) || (length (size (tree.Z)) ~= 2),
        warning ('Trees:NoTree', 'Z not vertical vector');
        if size (tree.Z, 1) ~= size (tree.dA, 1),
            warning ('Trees:NoTree', 'Z size not compatible with adjacency matrix');
        end
    end
end

if isfield (tree, 'D'),
    if (size (tree.D, 2) ~= 1) || (length (size (tree.D)) ~= 2),
        warning ('Trees:NoTree', 'D not vertical vector');
        if size (tree.D, 1) ~= size (tree.dA, 1),
            warning ('Trees:NoTree', 'D size not compatible with adjacency matrix');
        end
    end
end

if isfield (tree, 'R'),
    if (size (tree.R, 2) ~= 1) || (length (size (tree.R)) ~= 2),
        warning ('Trees:NoTree', 'R not vertical vector');
        if size (tree.R, 1) ~= size (tree.dA, 1),
            warning ('Trees:NoTree', 'R size not compatible with adjacency matrix');
        end
    end
end
