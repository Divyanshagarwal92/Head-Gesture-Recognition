/*
Last Update 26th Nov'14
Functionality:

1. broken down to functions
2. Calculates x-velocity and x-accelereations
3. Creates a out video (sampled) with acceleration-velocity display
 */ 

#include<iostream>
#include<cv.h>
#include<highgui.h>
#include<fstream>
#include<vector>
#include<utility>
#include<string>
#include<math.h>
using namespace std;
using namespace cv;
/*
#define XVELOCITY 0.05
#define YVELOCITY 0.05
#define XACCELERATION 0.05
#define YACCELERATION 0.05
*/
#define NUMPARTS 68
#define SKIP_RATE 5
#define SAMPLING 6 // SKIP_RATE * SAMPLING = 30fps
//#define NUM_FRAMES_CAPTURED 700 - 1
#define RATIO_NOSE 0.5
void facialLandmarks(string landmarkInfo, vector< vector< pair<int,int> > >& landmarks, int& numFramesCaptured)
{
	ifstream infile(&landmarkInfo[0]);
	//vector< vector< pair<int,int> > > landmarks; // max no of frames 100 and only one faces per frame 3
	vector<int> frameIndex;
	//int numFramesCaptured = 0;
	infile >> numFramesCaptured;
	cout << numFramesCaptured << endl;
	for( int i=0; i<numFramesCaptured; i++)
	{
		int f;
		infile >> f;
		frameIndex.push_back(f);
		vector< pair<int,int> > myvec;
		for( int parts=0; parts < NUMPARTS; parts++)
		{
			int r,c;
			string s2,s3;
			char c1;
			infile >> c1 >> r >> s2 >> c >> s3;
			myvec.push_back(make_pair(r,c));
		}
		landmarks.push_back(myvec);
	}
}
void artifacts(Point eyeMid, Point mouthMid, Point noseTip, Point noseBase, Mat& frame)
{
	circle(frame, eyeMid, 1 , CV_RGB(255,255,0), 1);
	circle(frame, mouthMid, 1 , CV_RGB(255,255,0), 1);
	circle(frame, noseBase, 1 , CV_RGB(255,255,0), 1);
	circle(frame, noseTip, 1 , CV_RGB(255,255,0), 1);
	line(frame, noseBase, noseBase + Point(10,0), CV_RGB(0,0,255),1);
	line(frame, noseBase, noseBase - Point(0,-10), CV_RGB(0,0,255),1);

/*	landmarks and other variables need to be defined 
	vector<pair<int,int> >::iterator it1;
	for( it1 = landmarks.at(order).begin(); it1 < landmarks.at(order).end(); it1++)
	{
		//cout << it1->first << " " << it1->second << endl;
		circle(frame, Point(it1->first, it1->second), 1 , CV_RGB(255,0,0), 1);
	}
	cout << "Frame: " << frameCount << endl;
*/	
}
void normalEstimates(double nVec[], Point eyeMid, Point mouthMid, Point noseTip, Point noseBase )
{
	double tilt = acos( (noseTip - noseBase).dot(Point(1,0))/norm(noseTip - noseBase) );
	double len_nose = norm(noseTip - noseBase);
	double len_face = norm(eyeMid - mouthMid);
	double m1 = pow(len_nose/len_face,2);
	double m2 = pow( (noseTip - noseBase).dot(Point(0,-1))/norm(noseTip - noseBase), 2);
	double Rn = RATIO_NOSE;
	double dz = ( Rn*Rn - m1 - 2*m2*Rn*Rn + sqrt( pow( m1 - Rn*Rn, 2) + 4*m1*m2*Rn*Rn) ) / ( 2*Rn*Rn*(1-m2) );
	dz = sqrt(dz);
	double sigma = acos( dz );
	nVec[0] = sin(sigma)*cos(tilt);
	nVec[1] = sin(sigma)*sin(tilt);
	nVec[2] = -cos(sigma);
	//cout << "Tilt : " << 180*tilt/3.14 << endl;
	//cout << "Theta : " <<180* acos(sqrt(m2))/3.14 << endl;
//	cout << (noseTip - noseBase).dot(Point(1,0))/norm(noseTip - noseBase) << "   " <<  (noseTip - noseBase).dot(Point(0,-1))/norm(noseTip - noseBase) << endl;
//	cout << nVec[0] << " " << nVec[1] << " " << nVec[2] << endl;
}
void displayOnFrame( double finalVelocity, double acceleration, Mat& frame)
{
	char velStr[100], accStr[100];   
	sprintf(velStr,"Velocity: %lf ", finalVelocity );  
	sprintf(accStr,"Acceleration: %lf ", acceleration );  
	if( finalVelocity > 0.05)
		putText(frame, velStr, Point(20,20), CV_FONT_NORMAL, 0.5, Scalar(255,255,0),1,1); //OutImg is Mat class;  
	else
		putText(frame, velStr, Point(20,20), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1); //OutImg is Mat class;  
	if( acceleration > 0.05)
		putText(frame, accStr, Point(20,40), CV_FONT_NORMAL, 0.5, Scalar(255,255,0),1,1); //OutImg is Mat class;  
	else
		putText(frame, accStr, Point(20,40), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1); //OutImg is Mat class;  
}
int main()
{
	string landmarkInfo;
	int numFramesCaptured = 0;
	vector< vector< pair<int,int> > > landmarks; // max no of frames 100 and only one faces per frame 3
	cout << "Facial Landmarks file: \n";
	cin >> landmarkInfo;
	
	//Get Facial Landmark Information for all the frames	
	facialLandmarks(landmarkInfo, landmarks, numFramesCaptured);
	
	string videoName;
	cout << "Video file: \n";
	cin >> videoName;
	string outputVideoName;
	cout << "OutputVideoFile: \n";
	cin >> outputVideoName;

	// open the video files for reading and writing
	cv::VideoCapture cap( videoName); 
	cv::VideoWriter output(outputVideoName, cap.get(CV_CAP_PROP_FOURCC),cap.get(CV_CAP_PROP_FPS),
			cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
	
	if (!output.isOpened())
	{
		std::cout << "!!! Output video could not be opened" << std::endl;
		return -1;
	}

	if ( !cap.isOpened() )
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}
	double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
	double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Height: " << height << " Width: " << width;
	cout << " Frame per seconds : " << fps << endl;
	cv::namedWindow("MyVideo"); //create a window called "MyVideo"
	cv::Mat frame;
	//iterates over the frames in video
	int frameCount = 0; 
	//iterates over the landmarks array
	int order = 0;
	double prevX = 0;
	double velocity = 0;
	double prevVelocity = 0;
	double finalVelocity = 0;
	double acceleration = 0;
	while(1)
	{
		
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}
		
		frameCount++;
		if( frameCount % SKIP_RATE != 0) // Process every kth frame
		{
			displayOnFrame(finalVelocity, acceleration, frame);
			output.write(frame);
			continue;
		}
		if( frameCount > SKIP_RATE * numFramesCaptured)
			break;

		Point eyeMid((landmarks.at(order).at(37).first + landmarks.at(order).at(46).first)/2,
				(landmarks.at(order).at(37).second + landmarks.at(order).at(46).second)/2 );
		Point mouthMid((landmarks.at(order).at(49).first + landmarks.at(order).at(55).first)/2, 
				(landmarks.at(order).at(49).second + landmarks.at(order).at(55).second)/2 );
		Point noseTip(landmarks.at(order).at(30).first, landmarks.at(order).at(30).second );
		Point noseBase( 0.6*mouthMid.x + 0.4*eyeMid.x , 0.6*mouthMid.y+ 0.4*eyeMid.y);

		//To plot various facial artifacts 		
		//artifacts( eyeMid, mouthMid, noseTip, noseBase, frame);
		
		// Normal Vector Estimation
		double nVec[3]; 
		normalEstimates( &nVec[0], eyeMid, mouthMid, noseTip, noseBase );

		
		velocity = velocity + abs( nVec[0] - prevX);
		prevX = nVec[0];
		order++;
		if( order % SAMPLING == 0 )
		{
			finalVelocity = velocity / SAMPLING;
			acceleration = abs( finalVelocity - prevVelocity);
			if( finalVelocity > 0.05)
				cout << "Second: " << order/SAMPLING <<  "  Velocity is: " << finalVelocity << endl;
			if( acceleration > 0.05)
				cout << "  Second: " << order/SAMPLING <<  " Acceleration is: " << acceleration << endl;
			displayOnFrame(finalVelocity, acceleration, frame);
			prevVelocity = finalVelocity;
			velocity = 0;
			//Print velocity to file
		}
		output.write(frame);
		imshow("Video", frame);
//		waitKey(0);

	}
	cap.release();
	output.release();
}


