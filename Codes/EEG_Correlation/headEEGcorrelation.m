% remove the first line from velocity_1.txt file
file = input('Filename: ', 's');

EEG = csvread(strcat(file,'_G.txt'));
% fp = fopen( strcat( file, '_velocity1.txt'));
% velocityInfo = textscan(fp,'%d %f %f %f %f');

f1 = fopen(strcat(file,'_normal.txt'));
positionInfo = textscan(f1,'%f %f %f');
meanX = mean(positionInfo{1})
stdX = std(positionInfo{1})

figure, plot(smooth(positionInfo{1},10));
title(' X position over all frames (Moving Avg)');
% disp(['In X dimension, Mean: ', num2str(meanX), ' and standard Deviation: ', num2str(stdX) ]);
 

% figure, plot(smooth(positionInfo{2}));
% title('Y position over all frames (Moving Avg)');
% disp(['In Y dimension, Mean: ', num2str(mean(positionInfo{2})), ' and standard Deviation: ', num2str(std(positionInfo{2})) ]);


PC1 = EEG(1,:);
% velocityX = velocityMode(positionInfo,10,10,30);
velocityX = velocityMean(positionInfo,10,10,30);

figure;
subplot(2,1,1);
plot(PC1(1:size(velocityX,2)));
title('PC1');

subplot(2,1,2);
plot(velocityX);
title('X velocity');
line([0,size(velocityX,2)],[mean(velocityX),mean(velocityX)],'Color',[1,0,0]);
saveas(gcf,strcat(file,'_velocityX_EEG.fig'));
meanVelocityX = mean(velocityX)
stdVelocityX = std(velocityX)
CorrelationVelX = corr2(PC1(1:size(velocityX,2)),velocityX)