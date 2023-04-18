function  process_beacon2_jitter()


% nr,time_epoch,len,srcgw,crc,rssi,snr,frequency,sf,cr,ftype,devaddr,fport,fcnt
% 1,1659362668.811991000,27,1,1,-108.0,0.0,867100000,11,5,2,654426274,8,36916

%close all;
%clear all;
%name='../loralog/csv/'; mode='utcshift';
%name='07_Brno_beacon'; mode='unix';

path='../loralog/csv/';

name='05_Wien_beacon';
M1 = readmatrix(strcat(path, name, '_valid.csv'), 'TreatAsMissing', 'NaN');
N1 = readmatrix(strcat(path, name, '_utcshift', '.csv'), 'TreatAsMissing', 'NaN');
numdays1 = days(datetime(M1(end,2), 'ConvertFrom', 'posixtime')-datetime(M1(1,2), 'ConvertFrom', 'posixtime'));
invalidstr1 = 'UTC shift'; 
shift1 = -18;

name='07_Brno_beacon';
M2 = readmatrix(strcat(path, name, '_valid.csv'), 'TreatAsMissing', 'NaN');
N2 = readmatrix(strcat(path, name, '_unix', '.csv'), 'TreatAsMissing', 'NaN');
numdays2 = days(datetime(M2(end,2), 'ConvertFrom', 'posixtime')-datetime(M2(1,2), 'ConvertFrom', 'posixtime'));
invalidstr2 = 'UNIX time'; 
shift2 = 315964782;

name='02_Liege_beacon';
M3 = readmatrix(strcat(path, name, '_valid.csv'), 'TreatAsMissing', 'NaN');
numdays3 = days(datetime(M3(end,2), 'ConvertFrom', 'posixtime')-datetime(M3(1,2), 'ConvertFrom', 'posixtime'));

name4title = 'Beacon jitter';
font = 12;

%% Timing jitter
figure();
edges = 0:2:400; col = 16;
c1 = round(histcounts(1e6*(M1(:,col)-0.154076), edges) ./ numdays1);
c2 = round(histcounts(1e6*(N1(:,col)-0.154076+shift1), edges) ./ numdays1);
c3 = round(histcounts(1e6*(M2(:,col)-0.154076), edges) ./ numdays2);
c4 = round(histcounts(1e6*(N2(:,col)-0.154076+shift2), edges) ./ numdays2);
c5 = round(histcounts(1e6*(M3(:,col)-0.154076), edges) ./ numdays3);
bar(edges(2:end), [c1' c3' c5' c2' c4'], 'Stacked', 'BarWidth', 1);
%bar(edges(2:end), c1, 'Stacked', 'BarWidth', 1);
%hold on;
%bar(edges(2:end), c2, 'Stacked', 'BarWidth', 1);
xlabel('Difference [\mus]'); ylabel('Packet count per day'); grid on;
legend('Wien valid', 'Brno valid', 'Liege valid', 'Wien UTC shift', 'Brno UNIX time');
%title(name4title,'Interpreter','none');
set(findall(gcf,'-property','FontSize'),'FontSize',font)
fig = gcf;
fig.Position(3)=fig.Position(3)*2;
fig.Position(4)=fig.Position(4)*1.3;
print(strcat(path,'00_jitter_all'), '-dpng');

