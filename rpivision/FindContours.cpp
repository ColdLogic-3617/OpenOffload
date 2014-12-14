#include <iostream>
#include <string>
#include "raspivid.h"
#include <sys/time.h>
#include <stdio.h> // fprintf()
#include <errno.h> // errno
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "WPILib/networktables/NetworkTable.h"

using namespace std;
using namespace cv;

static double kHorizontalFOVDeg = 31.37; // the pi's camera saw 61.5" wide at 109.5" distance: 2*ArcTan(30.75/109.5)=31.37 deg
                                         // the Axis camera provided by FIRST is 47.0 degrees
static double kVerticalFOVDeg = 480.0/640.0*kHorizontalFOVDeg; // both pi & Axis have a uniform angular FOV


int type = 0; // where to start
int defMin = 194;
int defMax = 255;
int defTall = 10;
int defNarr = 10;

unsigned long getmsofday() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec*1000 + tv.tv_usec/1000;
}

int main(int argc, char **argv) {

	// setup Network tables for this client to talk to the robot
	NetworkTable::SetClientMode();
	NetworkTable::SetIPAddress("10.36.18.2"); // where is the robot?
	NetworkTable *table = NetworkTable::GetTable("SmartDashboard"); // what table will we interface with?

	cout << "Got through the network tables\n";

	int width = 320;
	int height = 240;

	const float SeekWidth[] = {23.50, 4.0}; // inches
	const float SeekHeight[] = {4.0, 32.0}; // inches
	const float SeekRatio[2] = {SeekWidth[0] / SeekHeight[0], SeekWidth[1] / SeekHeight[1]}; // 24:18 = 1.333:1

	float alpha = 1.0; // constrast -- don't change it

	int c;
	opterr = 0; // "I'll create all the error messages, not getopt()"
	bool ShowMask = true; // flag to make the masked image where 'target' is seen, otherwise displays raw source image
	bool ShowVideo = false;   
	while ((c = getopt (argc, argv, "b:mvs")) != -1)
		switch (c) {
			case 'b': {
				char* endptr;
				errno = 0;    /* To distinguish success/failure after call */
				long val = strtol(optarg, &endptr, 0);

				/* Check for various possible errors */
				if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0) || (val > 100) || (val < 0)) {
					fprintf(stderr, "Invalid integer for 'b'eta: '%s'\n",optarg);
					exit(EXIT_FAILURE);
				}
				type = val;
				break;
			}
			case 'm':
				ShowMask = true;
				break;
			case 'v': // VGA resolution (640x480, but is slow)
				width = 640;
				height = 480;
				break;
			case 's': // Show the video
				ShowVideo = true;
				break;
			case '?':
				if (optopt == 'b')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				abort ();
		}

	cout << "Attempting to initialize capturing\n";

	RaspiVid v("/dev/video0", width, height);
	cout << "Calling constructor(s)\n";
	if (!v.initialize(RaspiVid::METHOD_MMAP)) {
		cout << "Unable to initialize!\n";
		return -1;
	}

	cout << "Successfully initialized!\n";

	v.setBrightness(50); // 10 for processing; 50 for visible image
	v.startCapturing();

	long start[10];

	if(ShowVideo) {
		namedWindow("Vision",1);
		//createTrackbar("Thresh Type", "Vision", &type, 4, NULL); // callback not needed
		//createTrackbar("Min Value", "Vision", &defMin, 255, NULL); // callback not needed
		//createTrackbar("Max Value", "Vision", &defMax, 255, NULL); // callback not needed
		createTrackbar("Percent Tall", "Vision", &defTall, 20, NULL); // callback not needed
		createTrackbar("Percent Narr", "Vision", &defNarr, 20, NULL); // callback not needed
	} else {
		table->PutNumber("Horizontal Percent Error", defTall);
		table->PutNumber("Vertical Percent Error", defNarr);
	}

	for (int i = 0; i<10; i++)
		start[i] = getmsofday(); // pre-load with 'now'
	for (int i = 0; 1; i++) {
		// Receive key-press updates, it is required if you want to output images,
		// so the task takes a moment to update the display.
		if (waitKey(1) > 0)
			break;

		string fileStream = "Mask"; // Default if no table present
		if (table->IsConnected()) {
			NetworkTable *StreamsTable = table->GetSubTable("File Streams");
			if (StreamsTable && StreamsTable->ContainsKey("selected")) {
				fileStream = StreamsTable->GetString("selected");
			}
		}

		ShowMask = (fileStream == "Mask");

		// Grab a frame from the vision API
		VideoBuffer buffer = v.grabFrame();

		// Put the frame into an OpenCV image matrix with a single color (gray scale)
		Mat image(height, width, CV_8UC1, buffer.data(), false); // AKA 'Y'

		Mat dst; // this will be a RGB version of the source image

		#if defined(YOU_WANT_RGB_COLOR_INSTEAD_OF_GREYSCALE)    
			// There is more data after the gray scale (Y) that contains U&V
			Mat cb(height/2, width/2, CV_8UC1, buffer.data()+(height*width), false); // 'U'
			Mat cr(height/2, width/2, CV_8UC1, buffer.data()+(height*width)*5/4, false); // 'V'

			// size up cb and cr to be same as y
			Mat CB;
			resize(cb,CB,cvSize(width,height));
			Mat CR;
			resize(cr,CR,cvSize(width,height));

			// empty image same as full (gray scale) image, but 3 channels:
			Mat ycbcr(height,width, CV_8UC3);

			Mat in[] = {image, CB, CR};
			int fromto[] = {0,0, 1,1, 2,2}; // YUV

			// mash 3 channels from 2 matrix into a single 3 channel matrix:
			mixChannels(in,3, &ycbcr,1, fromto,3);

			// convert that 3 channel YUV matrix into 3 channel RGB (displayable)
			cvtColor(ycbcr,image,CV_YCrCb2RGB);
			if (ShowMask) {
				dst = image.clone(); // make a copy, as we want dst to have the same RGB version
			}
		#else
			// After calculates, we want to draw 'on' the image, showing our results
			// graphically in some fashion -- that has to happen on a RGB
			if (ShowMask) {
				cvtColor(image,dst,CV_GRAY2RGB); // create CV_8UC3 version of same image
												 // which will allow us to draw some color on top of the gray
			}
		#endif    

		int Found = 0;
		if (!ShowMask) {
			// Show the original image with OpenCV on the screen (could be Grey or RGB)
			if(ShowVideo) {
				imshow("Vision", image);
			}

			if(fileStream == "Raw") {
				imwrite("/tmp/stream/pic.jpg", image);
			}

		}

		// alter the brightness to work better with contour finding
		//Mat new_image = image.clone();
		//image.copyTo(new_image);

		//Threshold the image into a new matrix with a minimum value of 1 and maximum of 255
		Mat thresh;
		//inRange(image, Scalar(1), Scalar(255), thresh);

		//Takes the source image and takes a thresh image 
		//applying a min search value and a max search value with a threshold type
		threshold(image, thresh, defMin, defMax, type);

		// Show the thresholded image with OpenCV on the screen

		if(ShowVideo) {
			imshow("Threshold", thresh);
		}

		if(fileStream == "Threshold") {
			imwrite("/tmp/stream/pic.jpg", thresh);
		}

		// Find all the contours in the thresholded image
		// The original thresholded image will be destroyed while finding contours
		vector <vector<Point> > contours;
		// CV_RETR_EXTERNAL retrieves only the extreme outer contours. 
		//                  It sets hierarchy[i][2]=hierarchy[i][3]=-1 for all the contours.
		// CV_CHAIN_APPROX_SIMPLE compresses horizontal, vertical, and diagonal segments and leaves only their end points. 
		//                        For example, an up-right rectangular contour is encoded with 4 points.
		findContours(thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		// Output information
		int prevFound = Found;
		for (int c=0; c<contours.size(); c++) {

			vector<Point> hull;
			convexHull(contours[c],hull,true);

			// examine each contours[c] for width / height, and if within 10% of SeekRatio keep it
			int MinX, MinY, MaxX, MaxY;
			MinX = MaxX = contours[c][0].x;
			MinY = MaxY = contours[c][0].y;
			#ifdef DEBUG		  
				cout << "[" << c << "].size()=" << contours[c].size() << ":";
			#endif		 
			for (int q=1; q<contours[c].size(); q++) {
				#ifdef DEBUG		  
					cout << contours[c][q] << ",";
				#endif
				MaxX = max(MaxX,contours[c][q].x);
				MaxY = max(MaxY,contours[c][q].y);
				MinX = min(MinX,contours[c][q].x);
				MinY = min(MinY,contours[c][q].y);
			} // now the extents of the contour (rectangle?) are [MinX,MinY]==>[MaxX,MaxY]
			int Width = MaxX - MinX;
			int Height = MaxY - MinY;	  

			if (Height > 10) { // at least 10 pixels, otherwise it's probably noise
				float ThisRatio = float(Width) / float(Height);

				cout << "W/H=(" << Width << " " << Height << " " << ThisRatio << ") ";
				// defNarr is the wide one
				// defTall is the tall one
				int narrErr = table->GetNumber("Horizontal Percent Error");
				int tallErr = table->GetNumber("Vertical Percent Error");
				if ((ThisRatio >= SeekRatio[0] * (1.0 - (narrErr / 100.0)) && ThisRatio <= SeekRatio[0] * (1.0 + (narrErr  / 100.0))) || (ThisRatio >= SeekRatio[1] * (1.0 - (tallErr  / 100.0)) && ThisRatio <= SeekRatio[1] * (1.0 + (tallErr / 100.0)))) {
					// close enough to say "this one could count"
					Found++;
					cout << "F";
					if (ShowMask) {
						// draw this contour on a copy of the image
						Scalar color( 0, 0, 255 );
						drawContours( dst, contours, c, color, CV_FILLED );
					}
				}
			}
		}
	}
	if (prevFound != Found) // This is purely for bandwidth savings. If there's no change, why broadcast?
		table->PutNumber("Hotness", Found);

	if (ShowVideo && ShowMask) {
		imshow("Vision", dst);
	}

	if(ShowMask) {
		imwrite("/tmp/stream/pic.jpg", dst);
	}

	long now = getmsofday();
	cout << "NumRects:" << Found << " "<< (10000 / (now-start[i%10])) << " FPS      \n";
	cout.flush();
	start[i%10] = now;
}
cout << "\n"; // save the last line of text
v.destroy();
}
