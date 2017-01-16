// EasyAR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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
using namespace std;
using namespace EasyAR;
int main()
{
	AugmentedTarget augmentTarget;
	Augmenter augmenter;
	CameraDevice cameraDevice;
	ImageTracker tracker;
	ImageTarget target;
	if (!initialize("tqn0tZt8ixgPS7n5lr8roGjatFJub3eDq6qI6bpABardEeGj90imbhJPjWj43Z1HEdg3P3R5cJTqOz7gI1QVOP8s6sod3tbVA3nE8ea892745534c440516acbe2d172870eegZCHZYSkjgRcwbKSJmJguPcU0avGbUTrfA98DahMot0nkKBrVKyRuJM6BStwNSLoSXr"))
	{
		cout << "Initialize failed!"<<endl;
	}
	//configure camera
	
	if (!cameraDevice.open(CameraDevice::Device::kDeviceDefault))
	{
		cout << "Open camera failed!" << endl;
	}
	//cameraDevice.setSize(Vec2I(1280, 720));
	Vec2I frameSize = cameraDevice.size();
	if (cameraDevice.isOpened())
	{
		if (frameSize[0] == 0 || frameSize[1] == 0)
		{
			cout << "Camera error" << endl;
			return -1;
		}
		cout << "frameSize: " << frameSize[0] << "*" << frameSize[1] << endl;
	}
	else {
		cout << "Camera not opened" << endl;
	}
	cameraDevice.cameraCalibration();

	cameraDevice.setFocusMode(CameraDevice::kFocusModeContinousauto);
	//add image tracker 
	if (!tracker.attachCamera(cameraDevice))
	{
		cout << "Add camera device to tracker failed" << endl;
	}
	tracker.setSimultaneousNum(1);
	//add target image
	std::string jstr="{\n"
			"	\"images\"	:\n"
			"	[\n"
			"		{\n"
			"			\"image\"	:	\"screen.png\",\n"
			"			\"name\" : \"screen\"\n"
			"    }\n"
			"  ]\n"
			"}";
	if (!target.load(jstr.c_str(), EasyAR::kStorageAssets | EasyAR::kStorageJson))
	{
		cout << "Add target image failed" << endl;
	}
	int id = target.id();
	cout << "image id: " << id << endl;
	const char* imageName = target.name();
	cout << "image name: " << imageName << endl;
	tracker.loadTargetBlocked(target);
	if (!tracker.start())
	{
		cout << "tarcker is not starting" << endl;
	}
	
	if (!augmenter.attachCamera(cameraDevice))
	{
		cout << "Attach camera to augmenter failed" << endl;
	}
	//augmenter.chooseAPI(EasyAR::Augmenter::API::kAugmenterAPIGL);
	augmenter.setViewPort(Vec4I(0, 0, 1280, 720));
	
	if (!cameraDevice.start())
	{
		cout << "Start to capture the frame failed!" << endl;
	}

//	while (1)
//	{

		Frame frame = augmenter.newFrame();
		Image image = frame.images()[0];

		int width = image.width();
		int height = image.height();
		cout << "frame image's size " << width << " * " << height << endl;

		if (augmentTarget.status() == AugmentedTarget::Status::kTargetStatusTracked)
		{
			cout << "Status: kTargetStatusDetected" << endl;
		}
		else if(augmentTarget.status() == AugmentedTarget::Status::kTargetStatusDetected)
		{
			cout << "Status: kTargetStatusDetected" << endl;
		}
		else if (augmentTarget.status() == AugmentedTarget::Status::kTargetStatusUnknown)
		{
			cout << "Status: kTargetStatusDetected" << endl;
		}
		else if (augmentTarget.status() == AugmentedTarget::Status::kTargetStatusUndefined)
		{
			cout << "Status: kTargetStatusUndefined" << endl;
		}
		frame.clear();
		image.clear();
		//quit 
	//	if (_getch() == 'q') //quit
	//	{

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
			if(augmenter.detachCamera(cameraDevice))
			{
				cout << "Deattache camera to augmenter successfully" << endl;
			}
			augmenter.clear();

	//	}
	//	Sleep(2);

//	}
	
	if (_getch() == 'q') //quit
	{
		return 0;
	}

	return 0;
}

