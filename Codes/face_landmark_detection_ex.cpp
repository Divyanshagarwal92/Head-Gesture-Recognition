#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>

#include <cv.h>
#include <highgui.h>
#include <dlib/opencv.h>
#include<fstream>

using namespace dlib;
using namespace std;
#define SKIP_RATE 1


int main(int argc, char** argv)
{  
	try
	{
		// This example takes in a shape model file and then a list of images to
		// process.  We will take these filenames in as command line arguments.
		// Dlib comes with example images in the examples/faces folder so give
		// those as arguments to this program.
		if (argc == 1)
		{
			cout << "Call this program like this:" << endl;
			cout << "./face_landmark_detection_ex shape_predictor_68_face_landmarks.dat faces/*.jpg" << endl;
			cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
			cout << "http://sourceforge.net/projects/dclib/files/dlib/v18.10/shape_predictor_68_face_landmarks.dat.bz2" << endl;
			return 0;
		}

		// We need a face detector.  We will use this to get bounding boxes for
		// each face in an image.
		frontal_face_detector detector = get_frontal_face_detector();
		// And we also need a shape_predictor.  This is the tool that will predict face
		// landmark positions given an image and face bounding box.  Here we are just
		// loading the model from the shape_predictor_68_face_landmarks.dat file you gave
		// as a command line argument.
		shape_predictor sp;
		deserialize(argv[1]) >> sp;
	
		ofstream myfile;
		string filename;
		cout << "Enter file to process: \n";
		cin >> filename;
		string landmarkInfo = filename + ".txt";
		myfile.open(&landmarkInfo[0]);
		int frameCount = 0;
		string videoName = filename + ".avi";
		cv::VideoCapture cap( videoName); // open the video file for reading
		if ( !cap.isOpened() )
		{
			cout << "Cannot open the video file" << endl;
			return -1;
		}
		double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
		double totalFramesInVideo = cap.get(CV_CAP_PROP_FRAME_COUNT);
		double capturedFrames = (int)(totalFramesInVideo/SKIP_RATE);
		cout << "Frame per seconds : " << fps << ",  Total Frames: " << totalFramesInVideo << ", capturedFrames: " << capturedFrames << endl;
		//cv::namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
		cv::Mat frame;

		myfile << capturedFrames  << endl;
		while(1)
		{	
			bool bSuccess = cap.read(frame); // read a new frame from video
			if (!bSuccess) //if not success, break loop
			{
				cout << "Cannot read the frame from video file" << endl;
				break;
			}
			frameCount++;
/*
			if( frameCount % SKIP_RATE != 0) // Process every kth frame
				continue;
			if( frameCount > SKIP_RATE * NUM_FRAMES_CAPTURED)
				break;
*/
			myfile << frameCount << endl;
			cout << "Processing frame " << frameCount << endl;
			array2d<rgb_pixel> img;
			assign_image(img, cv_image<bgr_pixel>(frame));
			//pyramid_up(img);

			// Now tell the face detector to give us a list of bounding boxes
			// around all the faces in the image.
			std::vector<rectangle> dets = detector(img);
			//cout << "Number of faces detected: " << dets.size() << endl;

			// Now we will go ask the shape_predictor to tell us the pose of
			// each face we detected.
			std::vector<full_object_detection> shapes;
			full_object_detection shape;
			if(dets.size() > 0)
			{
				for (unsigned long j = 0; j < 1; ++j) // Chk for just the first face
				{
					shape = sp(img, dets[j]);
					//cout << "number of parts: "<< shape.num_parts() << endl;
					//cout << "pixel position of first part:  " << shape.part(0) << endl;
					//cout << "pixel position of second part: " << shape.part(1) << endl;
					for( int parts = 0; parts < shape.num_parts(); parts++)
						myfile << shape.part(parts) << endl;
					// You get the idea, you can get all the face part locations if
					// you want them.  Here we just store them in shapes so we can
					// put them on the screen.
					shapes.push_back(shape);
				}
			}
			else
				shapes.push_back(shape);

			// Now lets view our face poses on the screen.
//			win.clear_overlay();
//			win.set_image(img);
//			win.add_overlay(render_face_detections(shapes));

                	//cv::imshow("MyVideo", frame); //show the frame in "MyVideo" window
			//cout << "Hit enter to process the next image..." << endl;
			//cin.get();
			//cv::waitKey(0);
		}
		myfile.close();

	}
	catch (exception& e)
	{
		cout << "\nexception thrown!" << endl;
		cout << e.what() << endl;
	}
}



