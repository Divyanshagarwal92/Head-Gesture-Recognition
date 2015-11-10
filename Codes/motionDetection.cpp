

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
#define XVELOCITY 0.08
#define WINDOW 15
/*
#define YVELOCITY 0.05
#define XACCELERATION 0.05
#define YACCELERATION 0.05
#define SKIP_RATE 5
#define SAMPLING 6 // SKIP_RATE * SAMPLING = 30fps
*/
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
void displayOnFrame( double xVelocity, double yVelocity, double xAcc, double yAcc, Mat& frame)
{
	char xvelStr[100], xaccStr[100];   
	char yvelStr[100], yaccStr[100];   
	sprintf(xvelStr,"X Velocity: %lf ", xVelocity );  
	sprintf(xaccStr,"X Acceleration: %lf ", xAcc );  
	sprintf(yvelStr,"Y Velocity: %lf ", yVelocity );  
	sprintf(yaccStr,"Y Acceleration: %lf ", yAcc );  
	if( xVelocity > 0.05)
		putText(frame, xvelStr, Point(20,20), CV_FONT_NORMAL, 0.5, Scalar(255,255,0),1,1); //OutImg is Mat class;  
	else
		putText(frame, xvelStr, Point(20,20), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1); //OutImg is Mat class;  
	if( yVelocity > 0.05)
		putText(frame, yvelStr, Point(300,20), CV_FONT_NORMAL, 0.5, Scalar(255,255,0),1,1); //OutImg is Mat class;  
	else
		putText(frame, yvelStr, Point(300,20), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1); //OutImg is Mat class;  
	if( xAcc > 0.05)
		putText(frame, xaccStr, Point(20,40), CV_FONT_NORMAL, 0.5, Scalar(255,255,0),1,1); //OutImg is Mat class;  
	else
		putText(frame, xaccStr, Point(20,40), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1); //OutImg is Mat class;  
	if( yAcc > 0.05)
		putText(frame, yaccStr, Point(300,40), CV_FONT_NORMAL, 0.5, Scalar(255,255,0),1,1); //OutImg is Mat class;  
	else
		putText(frame, yaccStr, Point(300,40), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1); //OutImg is Mat class;  
}
void velocity1(string velocity1Filename, int &numFramesProcessed, vector< vector<double> >& nVector)
{
	int numSeconds = numFramesProcessed / 30;
	ofstream velocityInfo(&velocity1Filename[0]);
	velocityInfo << numSeconds << endl;

	int frameCount = 0;
	cout << numFramesProcessed << endl;
	double prevXvelocity = 0;
	double prevYvelocity = 0;
	while(frameCount < numFramesProcessed)
	{
		double Xdisp1 = 0, Xdisp2 = 0;
		double Ydisp1 = 0, Ydisp2 = 0;
		for( int i = 0; i < WINDOW; i++)
		{
			Xdisp1 = Xdisp1 + nVector.at(frameCount).at(0);
			Ydisp1 = Ydisp1 + nVector.at(frameCount).at(1);
			frameCount++;
		}
		for( int i = 0; i < WINDOW; i++)
		{
			Xdisp2 = Xdisp2 + nVector.at(frameCount).at(0);
			Ydisp2 = Ydisp2 + nVector.at(frameCount).at(1);
			frameCount++;
		}
		double Xvelocity = abs(Xdisp1-Xdisp2)/WINDOW;
		double Yvelocity = abs(Ydisp1-Ydisp2)/WINDOW;
		double Xacc = Xvelocity - prevXvelocity;
		double Yacc = Yvelocity - prevYvelocity;
		//if(Xvelocity > XVELOCITY)
		velocityInfo << frameCount/30  << " " << Xvelocity << " " << Yvelocity << " " << Xacc << " " << Yacc << endl;
		prevXvelocity = Xvelocity;
		prevYvelocity = Yvelocity;
	}
	velocityInfo.close();
}
void outputVideo(string inputVideoname, string outputVideoname, string velocityFilename)
{

	// open the video files for reading and writing
	cv::VideoCapture cap(inputVideoname); 
	cv::VideoWriter output(outputVideoname, cap.get(CV_CAP_PROP_FOURCC),cap.get(CV_CAP_PROP_FPS),
			cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
	if (!output.isOpened())
	{
		std::cout << "!!! Output video could not be opened" << std::endl;
		return;
	}

	if ( !cap.isOpened() )
	{
		cout << "Cannot open the video file" << endl;
		return;
	}
	
// Reading from velocityInfo file
	ifstream velocityInfo(&velocityFilename[0]);
	int numSeconds;
	vector< vector<double> > motionVector;
	if(!velocityInfo.is_open() )
	{
		cout << "ERROR: Cant open normal information file \n";
		exit(1);
	}
	velocityInfo >> numSeconds;
	char c1;
	for( int i = 0; i < numSeconds; i++)
	{
		vector<double> myvec;
		double val;
		for( int j = 0; j < 5; j++ )
		{
			velocityInfo >> val;
			myvec.push_back(val);
		}
		motionVector.push_back(myvec);
	}
	velocityInfo.close();
// Writing Video
	double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
	double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Height: " << height << " Width: " << width;
	cout << " Frame per seconds : " << fps << endl;
	cv::namedWindow("MyVideo"); //create a window called "MyVideo"
	cv::Mat frame;
	int frameCount = 0;
	while(1)
	{
		
		int second = frameCount / 30;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}
		if( frameCount >= numSeconds*30)
			break;
		//if( frameCount % 5 != 0 )
		//	continue;
		displayOnFrame( motionVector.at(second).at(1), motionVector.at(second).at(2), 
				motionVector.at(second).at(3), motionVector.at(second).at(4),  frame);
		output.write(frame);
		frameCount++;
	}
	output.release();
	cap.release();

}
int main()
{
	string videoname;
	cout << "Video name: \n";
	cin >> videoname;
	string inputVideoname = videoname + ".avi";
	string outputVideoname = videoname + "_out.avi";
	string normalFilename = videoname + "_normal.txt";
	string velocity1Filename = videoname + "_velocity1.txt";
	cout << "Files: " << inputVideoname << "  " << outputVideoname << "   " << normalFilename << "  " << velocity1Filename<<  endl;
	
	//Getting the nVec's
	int numFramesCaptured;
	vector< vector<double> > nVector;
	ifstream normalInfo(&normalFilename[0]);
	if(!normalInfo.is_open() )
	{
		cout << "ERROR: Cant open normal information file \n";
		exit(1);
	}
	normalInfo >> numFramesCaptured;
	for( int i = 0; i < numFramesCaptured-1; i++)
	{
		vector<double> myvec;
		char c1;
		double val;
		for( int j = 0; j < 3; j++ )
		{
			normalInfo >> val;
			myvec.push_back(val);
		}
		normalInfo >> c1;
		nVector.push_back(myvec);
	}
	normalInfo.close();
	cout << numFramesCaptured << endl;

	int numFramesProcessed = numFramesCaptured - 1 - (numFramesCaptured-1)%30;
	velocity1(velocity1Filename, numFramesProcessed, nVector);
	outputVideo(inputVideoname, outputVideoname, velocity1Filename);

}


