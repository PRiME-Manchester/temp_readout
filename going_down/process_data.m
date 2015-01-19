deg{1}  = importdata('0deg.csv');
deg{2}  = importdata('5deg.csv');
deg{3}  = importdata('10deg.csv');
deg{4}  = importdata('15deg.csv');
deg{5}  = importdata('20deg.csv');
deg{6}  = importdata('RT_BoardOn_25.csv');
deg{7}  = importdata('30deg.csv');
deg{8}  = importdata('35deg.csv');
deg{9}  = importdata('40deg.csv');
deg{10} = importdata('45deg_b.csv');
deg{11} = importdata('50deg.csv');
deg{12} = importdata('55deg.csv');
deg{13} = importdata('60deg.csv');
deg{14} = importdata('65deg.csv');
deg{15} = importdata('70deg.csv');

data.temps = [0:5:70];

for chipNum=1:4
    for i=1:length(deg)
        data.chip(chipNum).temp1(:,i) = deg{i}.data(chipNum:4:end,4);
        data.chip(chipNum).temp2(:,i) = deg{i}.data(chipNum:4:end,5);
        data.chip(chipNum).temp3(:,i) = deg{i}.data(chipNum:4:end,6);
    end
end

save data

