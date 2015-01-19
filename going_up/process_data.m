deg{1}  = importdata('RT_BoardOn_26_3deg.csv');
deg{2}  = importdata('30deg.csv');
deg{3}  = importdata('35deg.csv');
deg{4}  = importdata('40deg.csv');
deg{5}  = importdata('45deg.csv');
deg{6}  = importdata('50deg.csv');
deg{7}  = importdata('55deg.csv');
deg{8}  = importdata('60deg.csv');
deg{9}  = importdata('65deg.csv');
deg{10} = importdata('70deg.csv');
deg{11} = importdata('75deg.csv');

data.temps = [26 30:5:75];

for chipNum=1:4
    for i=1:length(deg)
        data.chip(chipNum).temp1(:,i) = deg{i}.data(chipNum:4:end,4);
        data.chip(chipNum).temp2(:,i) = deg{i}.data(chipNum:4:end,5);
        data.chip(chipNum).temp3(:,i) = deg{i}.data(chipNum:4:end,6);
    end
end

