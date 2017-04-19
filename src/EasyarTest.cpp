// EasyAR.cpp : Defines the entry point for the console application.
//
#include "windows.h"
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "easyar/utility.hpp"
#include "easyar/imagetarget.hpp"
#include "easyar/image.hpp"
#include "easyar/camera.hpp"
#include "easyar/imagetracker.hpp"
#include "easyar/augmenter.hpp"
#include "easyar/frame.hpp"
#include "easyar/target.hpp"
#pragma comment(lib,"EasyAR.lib")
//add opencv header for test
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/flann/miniflann.hpp"
using namespace cv;
using namespace std;
using namespace EasyAR;
cv::Mat hwnd2mat();
int main()
{
	AugmentedTarget augmentTarget;
	Augmenter augmenter;
	CameraDevice cameraDevice;
	ImageTracker tracker;
	ImageTarget target;
	if (!initialize("tqn0tZt8ixgPS7n5lr8roGjatFJub3eDq6qI6bpABardEeGj90imbhJPjWj43Z1HEdg3P3R5cJTqOz7gI1QVOP8s6sod3tbVA3nE8ea892745534c440516acbe2d172870eegZCHZYSkjgRcwbKSJmJguPcU0avGbUTrfA98DahMot0nkKBrVKyRuJM6BStwNSLoSXr"))
	{
		std::cout << "Initialize failed!" << endl;
	}

	if (!cameraDevice.open(2))
	{
		std::cout << "Open camera failed!" << endl;
	}

	cameraDevice.setSize(Vec2I(640, 480));
	Vec2I frameSize = cameraDevice.size();
	if (cameraDevice.isOpened())
	{
		if (frameSize[0] == 0 || frameSize[1] == 0)
		{
			std::cout << "Camera error" << endl;
			return -1;
		}
		std::cout << "frameSize: " << frameSize[0] << "*" << frameSize[1] << endl;
	}
	else {
		std::cout << "Camera not opened" << endl;
	}

	CameraCalibration calibration = cameraDevice.cameraCalibration();
	calibration.focalLength()[0] = 6.1287779109201915e+02;
	calibration.focalLength()[1] = 6.1287779109201915e+02;
	calibration.principalPoint()[0] = 320;
	calibration.principalPoint()[1] = 240;
	calibration.distortionParameters()[0] = 1.3058699547349051e-01;
	calibration.distortionParameters()[1] = -3.8614805620030163e-01;
	calibration.distortionParameters()[2] = 0;
	calibration.distortionParameters()[3] = 0;

	cameraDevice.setHorizontalFlip(true);
	cameraDevice.setFocusMode(CameraDevice::kFocusModeContinousauto);
	//add image tracker 
	if (!tracker.attachCamera(cameraDevice))
	{
		std::cout << "Add camera device to tracker failed" << endl;
	}
	tracker.setSimultaneousNum(1);

	if (!augmenter.attachCamera(cameraDevice))
	{
		std::cout << "Attach camera to augmenter failed" << endl;
	}
	//augmenter.chooseAPI(EasyAR::Augmenter::API::kAugmenterAPIGL);
	augmenter.setViewPort(Vec4I(0, 0, 1280, 720));

	if (!cameraDevice.start())
	{
		std::cout << "Start to capture the frame failed!" << endl;
	}

	int count = 0;
	while (1)
	{
		if (0==count || (count % 10) == 0)
		{
			cv::Mat desktopImage = hwnd2mat();
			cv::imwrite("../bin/debug/desktop.jpg", desktopImage);
			namedWindow("desktopImage", WINDOW_AUTOSIZE);
			imshow("desktopImage", desktopImage);

			std::string jstr = "{\n"
				"	\"images\"	:\n"
				"	[\n"
				"		{\n"
				"			\"image\"	:	\"desktop.jpg\",\n"
				"			\"name\" : \"desktop\"\n"
				"    }\n"
				"  ]\n"
				"}";
			if (!target.load(jstr.c_str(), EasyAR::kStorageAssets | EasyAR::kStorageJson))
				std::cout << "Add target image failed" << endl;

			int id = target.id();
			const char* imageName = target.name();
			tracker.loadTargetBlocked(target);
			if (!tracker.start())
			{
				std::cout << "tarcker is not starting" << endl;
			}
		}
		count++;

		Frame frame = augmenter.newFrame();
		Image image = frame.images()[0];
		if (frame.images().size() <= 0)
			continue;

		int width = image.width();
		int height = image.height();
		//std::cout << "frame image's size " << width << " * " << height << endl;
		int strip = image.stride();
		//std::cout << "strip: " << strip << endl;
		int pixelFormat = image.format();

		Mat cameraImage = Mat(height, width, CV_8UC3);
		cameraImage.data = (uchar *)image.data();
		namedWindow("cameraImage", WINDOW_AUTOSIZE);
		imshow("cameraImage", cameraImage);
		waitKey(10);

		AugmentedTarget::Status status = frame.targets()[0].status();
		if (status == AugmentedTarget::Status::kTargetStatusTracked)
		{
			std::cout << "Status: kTargetStatusTracked" << endl;
			Matrix44F projectionMatrix = getProjectionGL(cameraDevice.cameraCalibration(), 0.2f, 500.f);
			Matrix44F pose_mat = getPoseGL(frame.targets()[0].pose());

			std::cout << "Pose Matrix:" << std::endl;
			for (int row = 0; row < 4; row++)
			{
				for (int col = 0; col < 4; col++)
					std::cout << pose_mat.data[row * 4 + col] << ", ";
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		else if (status == AugmentedTarget::Status::kTargetStatusDetected)
		{
			std::cout << "Status: kTargetStatusDetected" << endl;
		}
		else if (status == AugmentedTarget::Status::kTargetStatusUnknown)
		{
			std::cout << "Status: kTargetStatusUnknown" << endl;
		}
		else if (status == AugmentedTarget::Status::kTargetStatusUndefined)
		{
			std::cout << "Status: kTargetStatusUndefined" << endl;
		}

		frame.clear();
		image.clear();

#if 0
		//quit 
		if (_getch() == 'q') //quit
		{
			if (cameraDevice.stop())
			{
				cout << "Stop capture the frame!" << endl;
			}
			if (cameraDevice.close())
			{
				cout << "Closed the camera" << endl;
			}
			if (tracker.detachCamera(cameraDevice))
			{
				cout << "detached the camera device" << endl;
			}
			if (tracker.stop())
			{
				cout << "Stoped the tracker successfully" << endl;
			}
			if (tracker.stop())
			{
				cout << "Stopped the tracker" << endl;
			}
			tracker.unloadTargetBlocked(target);
			if (augmenter.detachCamera(cameraDevice))
			{
				cout << "Deattache camera to augmenter successfully" << endl;
			}
			augmenter.clear();
	}
#endif

}

	if (_getch() == 'q') //quit
	{
		return 0;
	}

	return 0;
}

cv::Mat hwnd2mat()
{

	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	cv::Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(GetDesktopWindow());
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(GetDesktopWindow(), &windowsize);

	srcheight = 1080;
	srcwidth = 1920;
	height = windowsize.bottom / 4;  //change this to whatever size you want to resize to
	width = windowsize.right / 4;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

																										 // avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(GetDesktopWindow(), hwindowDC);

	return src;
}