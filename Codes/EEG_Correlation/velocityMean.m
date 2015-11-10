function [velocityX] = velocityMean(positionInfo,WINDOW1,WINDOW2,FPS)

% WINDOW1 = Averaging window for positionX
% WINDOW2 = SKIP frame rate for velocity.
positionX = positionInfo{1};
positionY = positionInfo{2};


smoothX = smooth(positionX,WINDOW1);
smoothY = smooth(positionY,WINDOW1);

frames = size(positionX,1);

ref = 0;
distX = 0;
k = 1; 
velocityX = [];
for i=1:frames    
    if mod(i,WINDOW2) == 0 
        distX = distX + abs(smoothX(i)-ref);
        ref = smoothX(i);
    end
    if mod(i, FPS) == 0
        velocityX = [ velocityX, distX ];
        distX = 0;
    end
end