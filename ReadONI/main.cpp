#include <opencv2/opencv.hpp>
#include <XnCppWrapper.h>

int main(int argc, char **argv) {

	if(argc < 2) {
		std::cout << "ReadONIFiles.exe file.oni" << std::endl;
		return 0;
	}

	xn::Context context;
	xn::Player player;
	xn::ImageGenerator imageGenerator;
	xn::DepthGenerator depthGenerator;
	xn::ImageMetaData imageMap;
	xn::DepthMetaData depthMap;
	XnPixelFormat pixelFormat;
	XnRGB24Pixel* imageData;
	XnDepthPixel* depthData;
	cv::Mat colorImage;
	cv::Mat depthImage;

	context.Init();
	context.OpenFileRecording(argv[1], player);
	player.SetRepeat(false);
	imageGenerator.Create(context);
	pixelFormat = imageGenerator.GetPixelFormat();
	if(pixelFormat != XN_PIXEL_FORMAT_RGB24) imageGenerator.SetPixelFormat(XN_PIXEL_FORMAT_RGB24);
	
	imageGenerator.GetMetaData(imageMap);
	XnUInt32 fps = imageMap.FPS();
	XnUInt32 height = imageMap.YRes();
	XnUInt32 width = imageMap.XRes();
	
	depthGenerator.Create(context);
	XnUInt32 numberFrames;
	player.GetNumFrames(depthGenerator.GetName(), numberFrames);

	cv::namedWindow("RGB");
	cv::namedWindow("Depth");
	cv::Mat onlyToShowDepthImage;

	for(int frame = 0; frame < numberFrames; frame++) {
		
		imageGenerator.WaitAndUpdateData();
		imageGenerator.GetMetaData(imageMap);
		imageData = const_cast<XnRGB24Pixel*>(imageMap.RGB24Data());
		colorImage = cv::Mat(height, width, CV_8UC3, reinterpret_cast<void*>(imageData));
		cv::cvtColor(colorImage, colorImage, CV_BGR2RGB);

		depthGenerator.WaitAndUpdateData();
		depthGenerator.GetMetaData(depthMap);
		depthData = const_cast<XnDepthPixel*>(depthMap.Data());
		depthImage = cv::Mat(height, width, CV_16U, reinterpret_cast<void*>(depthData));

		onlyToShowDepthImage = cv::Mat(height, width, CV_8UC3);
		for(int pixel = 0; pixel < width * height; pixel++)
			for(int ch = 0; ch < 3; ch++)
				onlyToShowDepthImage.ptr<unsigned char>()[pixel * 3 + ch] = (depthImage.ptr<unsigned short>()[pixel]/2048.0) * 255;

		cv::imshow("RGB", colorImage);
		cv::imshow("Depth", onlyToShowDepthImage);
		cvWaitKey(20);

	}

	context.StopGeneratingAll();
	context.Release();

	return 0;
}