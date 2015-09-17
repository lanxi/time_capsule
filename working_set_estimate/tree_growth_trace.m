

% parameters setting
bits_per_node = 128;
bits_per_tree = 32;
percentage_ = 0.95;
Mega_Byte = 8 * 1024 * 1024; % one MB in bits







tree_growth_matrix = load('tree_growth_trace.data'); % [timestamp treeId treeSize treeNumber forestSize]



tree_growth = struct();
forest_growth_struct = struct();
tree_number_struct = struct();



% aggregate all the trace at the same timestamp
rowNumber = size(tree_growth_matrix,1);


% [timeStamps,indeces] = unique(tree_growth_matrix(:,1),'last');
% 
% forest_growth = [timeStamps,tree_growth_matrix(indeces,5)];
% tree_number_growth = [timeStamps,tree_growth_matrix(indeces,4)];
% 
% 
% fig1 = figure(1);
% plot(forest_growth(:,1),forest_growth(:,2));
% fig2 = figure(2);
% plot(tree_number_growth(:,1),tree_number_growth(:,2));




forest_growth_struct.(['time',num2str(tree_growth_matrix(1,1))]) = tree_growth_matrix(1,5);
tree_number_struct.(['time',num2str(tree_growth_matrix(1,1))]) = tree_growth_matrix(1,4);

for rowId = 2:rowNumber
    
        
    forest_growth_struct.(['time',num2str(tree_growth_matrix(rowId,1))]) = tree_growth_matrix(rowId,5);
    tree_number_struct.(['time',num2str(tree_growth_matrix(rowId,1))]) = tree_growth_matrix(rowId,4);

    if tree_growth_matrix(rowId,1) == tree_growth_matrix(rowId - 1,1) && tree_growth_matrix(rowId,2) == tree_growth_matrix(rowId - 1,2)
        if rowId == rowNumber
            if isfield(tree_growth,['tree',num2str(tree_growth_matrix(rowId,2))])
                tree_growth.(['tree',num2str(tree_growth_matrix(rowId,2))]) = [tree_growth.(['tree',num2str(tree_growth_matrix(rowId,2))]);[tree_growth_matrix(rowId,1) tree_growth_matrix(rowId,3)]];
            else
                tree_growth.(['tree',num2str(tree_growth_matrix(rowId,2))]) = [tree_growth_matrix(rowId,1) tree_growth_matrix(rowId,3)];
            end
        end
    else
        if rowId == rowNumber
            if isfield(['tree',num2str(tree_growth_matrix(rowId,2))])
                tree_growth.(['tree',num2str(tree_growth_matrix(rowId,2))]) = [tree_growth.(['tree',num2str(tree_growth_matrix(rowId,2))]);[tree_growth_matrix(rowId,1) tree_growth_matrix(rowId,3)]];
            else
                tree_growth.(['tree',num2str(tree_growth_matrix(rowId,2))]) = [tree_growth_matrix(rowId,1) tree_growth_matrix(rowId,3)];
            end
        end
        
        if isfield(tree_growth,['tree',num2str(tree_growth_matrix(rowId - 1,2))])
            tree_growth.(['tree',num2str(tree_growth_matrix(rowId - 1,2))]) = [tree_growth.(['tree',num2str(tree_growth_matrix(rowId - 1,2))]);[tree_growth_matrix(rowId - 1,1) tree_growth_matrix(rowId - 1,3)]];
        else
            tree_growth.(['tree',num2str(tree_growth_matrix(rowId - 1,2))]) = [tree_growth_matrix(rowId - 1,1) tree_growth_matrix(rowId - 1,3)];
        end
            
    end
end




% estimate working set
trees = fieldnames(tree_growth);
treeNumber = length(trees);

treeDeleteTimeTable = zeros(treeNumber,2); % treeId time

% get the time when certain tree reaches the preset percentage of its total
% size

for treeId = 1:treeNumber
    index = find(tree_growth.(trees{treeId})(:,2) >= percentage_ * tree_growth.(trees{treeId})(end,2),1,'first');
    
    treeDeleteTimeTable(treeId,2) = tree_growth.(trees{treeId})(index,1);
    treeDeleteTimeTable(treeId,1) = str2num(trees{treeId}(5:end));
end

% sort the above table according to the time sequence
[~,orders] = sort(treeDeleteTimeTable(:,2));

treeDeleteTimeTable(:,1) = treeDeleteTimeTable(orders,1);
treeDeleteTimeTable(:,2) = treeDeleteTimeTable(orders,2);


% estimate working set
bits_required_ = zeros(treeNumber,2); % time bits_required


deleteNodeNumber = 0;
for i = 1:treeNumber
    deleteNodeNumber = deleteNodeNumber + tree_growth.(['tree',num2str(treeDeleteTimeTable(i,1))])(end,2);
    bits_required_(i,1) = treeDeleteTimeTable(i,2);
    bits_required_(i,2) = (forest_growth_struct.(['time',num2str(treeDeleteTimeTable(i,2))]) - deleteNodeNumber) * bits_per_node + ...
        tree_number_struct.(['time',num2str(treeDeleteTimeTable(i,2))]) * bits_per_tree;
    
end

[values,indeces] = unique(bits_required_(:,1),'last');
bits_required_final = [values,bits_required_(indeces,2)];

[~,orders] = sort(bits_required_final(:,1));
bits_required_final(:,1) = bits_required_final(orders,1);
bits_required_final(:,2) = bits_required_final(orders,2) / Mega_Byte;

plot(bits_required_final(:,1),bits_required_final(:,2));
    











% fig = figure(1);


% tree_size_distribution = zeros(treeNumber,1);
% for i = 1:treeNumber
% %     plot(tree_growth.(trees{i})(:,1),tree_growth.(trees{i})(:,2));
% %     hold on
% tree_size_distribution(i) = tree_growth.(trees{i})(2,end);
% end


