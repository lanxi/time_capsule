
fig = figure(1);

for i = 55:5:95
    data = load(['working_set.dat-',num2str(i)]);
    plot(data(2:end,1),data(2:end,2) / data(end,2));
    hold on
end