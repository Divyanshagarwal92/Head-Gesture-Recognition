function [velocityX] = velocityMode(positionInfo,WINDOW1,WINDOW2,FPS)
% file = input('Filename: ', 's');
% f1 = fopen(strcat(file,'_normal.txt'));
% positionInfo = textscan(f1,'%f %f %f'); 
% WINDOW1 = 10;
% WINDOW2 = 15;
% FPS = 30;

positionX = positionInfo{1};
positionY = positionInfo{2};


smoothX = smooth(positionX,WINDOW1);
smoothY = smooth(positionY,WINDOW1);

frames = size(positionX,1);

ref = 0;
distX = [];
k = 1; 
velocityX = [];
for i=1:frames    
    if mod(i,WINDOW2) == 0 
        distX = [distX , abs(smoothX(i)-ref)];
        ref = smoothX(i);
    end
    if mod(i, FPS) == 0
        max_dist = mode(distX);
        velocityX = [ velocityX, max_dist ];
        distX = [];
    end
end