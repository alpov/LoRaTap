
%close all; process_valid('../loralog/csv/05_Wien_valid');

%%
%close all; process_all('../loralog/csv/05_Wien_all');

%%
tic
close all; process_all('../loralog/csv/01_Brno_all');
close all; process_valid('../loralog/csv/01_Brno_valid');
close all; process_all('../loralog/csv/02_Liege_all');
close all; process_valid('../loralog/csv/02_Liege_valid');
close all; process_all('../loralog/csv/03_Brno_join_all');
close all; process_valid('../loralog/csv/03_Brno_join_valid');
close all; process_all('../loralog/csv/04_Graz_all');
close all; process_valid('../loralog/csv/04_Graz_valid');
close all; process_all('../loralog/csv/05_Wien_all');
close all; process_valid('../loralog/csv/05_Wien_valid');
close all; process_beacon('../loralog/csv/05_Wien_beacon', 'utcshift');
close all; process_beacon_ts('../loralog/csv/05_Wien_beacon_all');
close all; process_all('../loralog/csv/07_Brno_all');
close all; process_valid('../loralog/csv/07_Brno_valid');
close all; process_beacon('../loralog/csv/07_Brno_beacon', 'unix');
close all;
toc
