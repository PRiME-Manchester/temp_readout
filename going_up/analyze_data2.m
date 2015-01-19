load data

h=figure;
subplot(1,3,1);
hold all
for chip=1:4
    Y = data.chip(chip).temp1;
    %plot(data.temps, mean(Y, 1), '.-');
    errorbar(data.temps, mean(Y,1), std(Y,1));
    
    title(['Temp Sensor 1']);
    xlabel('T/deg C');
end
legend('Chip 1', 'Chip 2', 'Chip 3', 'Chip 4');

subplot(1,3,2);
hold all
for chip=1:4
    Y = data.chip(chip).temp2;
    %plot(data.temps, mean(Y, 1), '.-');
    errorbar(data.temps, mean(Y,1), std(Y,1));
    
    title(['Temp Sensor 2']);
    xlabel('T/deg C');
end
legend('Chip 1', 'Chip 2', 'Chip 3', 'Chip 4');

subplot(1,3,3);
hold all
for chip=1:4
    Y = data.chip(chip).temp3;
    %plot(data.temps, mean(Y, 1), '.-');
    errorbar(data.temps, mean(Y,1), std(Y,1));
    
    title(['Temp Sensor 3']);
    xlabel('T/deg C');
end
legend('Chip 1', 'Chip 2', 'Chip 3', 'Chip 4');


%%
for chip=1:4
    figure;
    subplot(1,3,1);
    hold all
    for i=1:size(data.chip(chip).temp1,2)
        plot(0:0.5:300, data.chip(chip).temp1(:,i), '.');
    end
    legend('26','30', '35', '40', '45', '50', '55', '60', '65', '70', '75');
    xlim([0 300]);
    title(['Chip' num2str(chip) ' - Temp Sensor 1']);
    xlabel('Time [s]');
    hold off

    subplot(1,3,2);
    hold all
    for i=1:size(data.chip(chip).temp2,2)
        plot(0:0.5:300, data.chip(chip).temp2(:,i), '.');
    end
    legend('26','30', '35', '40', '45', '50', '55', '60', '65', '70', '75');
    xlim([0 300]);
    title(['Chip' num2str(chip) ' - Temp Sensor 2']);
    xlabel('Time [s]');
    hold off

    subplot(1,3,3);
    hold all
    for i=1:size(data.chip(chip).temp3,2)
        plot(0:0.5:300, data.chip(chip).temp3(:,i), '.');
    end
    legend('26','30', '35', '40', '45', '50', '55', '60', '65', '70', '75');
    xlim([0 300]);
    title(['Chip' num2str(chip) ' - Temp Sensor 3']);
    xlabel('Time [s]');
    hold off
end
