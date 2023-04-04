function  process_all(name)

% nr,time_epoch,len,srcgw,crc,rssi,snr,frequency,sf,cr,ftype
% 1,1659362668.811991000,27,1,1,-108.0,0.0,867100000,11,5,2

%close all;
%clear all;
%name = '05_Wien_all';

M = readmatrix(strcat(name, '.csv'), 'TreatAsMissing', 'NaN');
numdays = days(datetime(M(end,2), 'ConvertFrom', 'posixtime')-datetime(M(1,2), 'ConvertFrom', 'posixtime'));

% Extract city and type from filename
[~, filename, ~] = fileparts(name);
[city, type] = strtok(filename(4:end), '_');
type = type(2:end);

% Replace underscores with spaces and format output string
type = strrep(type, '_', ' ');
name4title = sprintf('%s (%s)', city, type);

%% Histogram of spreading factor usage of all packets
figure();
edges = 6.5:1:12.5; col = 9;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
c = c1+c2+c3;
bar([c1' c2' c3'], 'Stacked', 'BarWidth', 0.7);
set(gca, 'xticklabel', {'SF7', 'SF8', 'SF9', 'SF10', 'SF11', 'SF12'});
xlabel('Spreading factor'); ylabel('Packet count per day'); grid on;
text(1:length(c), c, num2str(c'), 'vert', 'bottom', 'horiz', 'center');
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_01'), '-dpng');

%% Histogram of coding ratio
figure();
edges = 4.5:1:8.5; col = 10;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
c = c1+c2+c3;
bar([c1' c2' c3'], 'Stacked', 'BarWidth', 0.7);
set(gca, 'xticklabel', {'4/5', '4/6', '4/7', '4/8'});
%set(gca, 'yscale', 'log');
xlabel('Coding ratio'); ylabel('Packet count per day'); grid on;
text(1:length(c), c, num2str(c'), 'vert', 'bottom', 'horiz', 'center');
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_02'), '-dpng');

%% Histogram of frequency channels usage of all packets
figure();
edges = 1e6.*[867.0 867.2 867.4 867.6 867.8 868.0 868.2 868.4 868.6 869.6]; col = 8;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
c = c1+c2+c3;
bar([c1' c2' c3'], 'Stacked', 'BarWidth', 0.7);
set(gca, 'xticklabel', {'867.1', '867.3', '867.5', '867.7', '867.9', '868.1', '868.3', '868.5', '869.525'});
xlabel('Frequency [MHz]'); ylabel('Packet count per day'); grid on;
text(1:length(c), c, num2str(c'), 'vert', 'bottom', 'horiz', 'center');
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_03'), '-dpng');

%% Histogram of RSSI
figure();
edges = -131:2:-49; col = 6;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
bar(-130:2:-50,[c1' c2' c3'], 'Stacked', 'BarWidth', 1);
xlabel('RSSI [dBm]'); ylabel('Packet count per day'); grid on;
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_04'), '-dpng');

%% Histogram of SNR
figure();
edges = -25.5:1:15.5; col = 7;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
bar(-25:1:15, [c1' c2' c3'], 'Stacked', 'BarWidth', 1);
xlabel('SNR [dBm]'); ylabel('Packet count per day'); grid on;
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_05'), '-dpng');

%% Histogram of packet length
figure();
edges = [0 11.5:4:55.5 255]; col = 3;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
c = c1+c2+c3;
bar([c1' c2' c3'], 'Stacked', 'BarWidth', 0.7);
xticks(1:length(edges)-1);
set(gca, 'xticklabel', {'<12', '12-15', '16-19', '20-23', '24-27', '28-31', '31-34', '35-39', '40-43', '44-47', '48-51', '52-55', '>55'});
xlabel('Data length [B]'); ylabel('Packet count per day'); grid on;
text(1:length(c), c, num2str(c'), 'vert', 'bottom', 'horiz', 'center');
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_06'), '-dpng');

%% Histogram of source gateways
figure();
edges = [0.5:1:3.5]; col = 4;
c1 = round(histcounts(M(M(:,5)==1,col), edges) ./ numdays);
c2 = round(histcounts(M(M(:,5)==2,col), edges) ./ numdays);
c3 = round(histcounts(M(M(:,5)==4,col), edges) ./ numdays);
c = c1+c2+c3;
bar([c1' c2' c3'], 'Stacked', 'BarWidth', 0.7);
set(gca, 'xticklabel', {'Uplink', 'Downlink RX1', 'Downlink RX2'});
%set(gca, 'yscale', 'log');
xtickangle(45);
xlabel('Source Gateway'); ylabel('Packet count per day'); grid on;
text(1:length(c), c, num2str(c'), 'vert', 'bottom', 'horiz', 'center');
legend('CRC OK', 'CRC Bad', 'No CRC');
title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',8)
print(strcat(name, '_07'), '-dpng');

