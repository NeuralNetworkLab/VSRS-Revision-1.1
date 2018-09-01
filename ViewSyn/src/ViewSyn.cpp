/*
* View Synthesis Reference Software
* ****************************************************************************
* Copyright (c) 2011/2013 Poznan University of Technology
*
* Address:
*   Poznan University of Technology,
*   Polanka 3 Street, Poznan, Poland
*
* Authors:
*   Krysztof Wegner     <kwegner@multimedia.edu.pl>
*   Olgierd Stankiewicz <ostank@multimedia.edu.pl>
*
* You may use this Software for any non-commercial purpose, subject to the
* restrictions in this license. Some purposes which can be non-commercial are
* teaching, academic research, and personal experimentation. You may also
* distribute this Software with books or other teaching materials, or publish
* the Software on websites, that are intended to teach the use of the
* Software.
*
* Reference to the following source document:
*
* Takanori Senoh, Kenji Yamamoto, Nobuji Tetsutani, Hiroshi Yasuda, Krzysztof Wegner,
* "View Synthesis Reference Software (VSRS) 4.2 with improved inpainting and hole filing"
* ISO/IEC JTC1/SC29/WG11 MPEG2017/M40xx April 2017, Hobart, Australia
*
* are required in all documents that report any usage of the software.

* You may not use or distribute this Software or any derivative works in any
* form for commercial purposes. Examples of commercial purposes would be
* running business operations, licensing, leasing, or selling the Software, or
* distributing the Software for use with commercial products.
* ****************************************************************************
*/
//#include <string.h>
#include <time.h>

#include "version.h"
#include "yuv.h"
#include "ParameterViewInterpolation.h"
#include "ViewInterpolation.h"
#include<string>

using namespace std;

#ifndef WIN32
#define BYTE unsigned char
#endif

int main(int argc, char *argv[])
{
	unsigned int n;

	CParameterViewInterpolation  cParameter;

	CViewInterpolation cViewInterpolation;
	CIYuv<ImageType> yuvBuffer;

	cv::Mat bgrImg;
#ifdef OUTPUT_COMPUTATIONAL_TIME
	clock_t start, finish, first;
	first = start = clock();
#endif
	printf(argv[0]);
	printf("\nView Synthesis Reference Software (modified version), Version %.2f\n", VERSION);
	printf("MPEG-I Visual, April 2017\n");
	printf("Revision by Nerul Network and Image processing Lab of SWUST\n\n");

	if (cParameter.Init(argc, argv) != 1) return 0;    //check parameters and save these parameters
	if (!cViewInterpolation.Init(cParameter)) return 10;    //initial parameters 

	if (!yuvBuffer.resize(cParameter.getSourceHeight(), cParameter.getSourceWidth(), 420)) return 2;  //get yuv data of picture and allocate memory

	FILE *fin_view_r, *fin_view_l, *fin_depth_r, *fin_depth_l, *fout;

	if ((fin_view_l = fopen(cParameter.getLeftViewImageName().c_str(), "rb")) == NULL ||
		(fin_view_r = fopen(cParameter.getRightViewImageName().c_str(), "rb")) == NULL ||
		(fin_depth_l = fopen(cParameter.getLeftDepthMapName().c_str(), "rb")) == NULL ||
		(fin_depth_r = fopen(cParameter.getRightDepthMapName().c_str(), "rb")) == NULL ||
		(fout = fopen(cParameter.getOutputVirViewImageName().c_str(), "wb")) == NULL)          // 此处获取输出文件名
	{
		fprintf(stderr, "Can't open input file(s)\n");
		return 3;
	}

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	printf("Initialization: %.4f sec\n", (double)(finish - start) / CLOCKS_PER_SEC);
	start = finish;
#endif

	// 循环读取没一帧，由于我们直接做图像插值，所以只读取一帧即可
	//for(n = cParameter.getStartFrame(); n < cParameter.getStartFrame() + cParameter.getNumberOfFrames(); n++) 
	//{
	n = cParameter.getStartFrame();
	printf("frame number = %d \n", n);

	// 读取深度图的一帧
	if (!cViewInterpolation.getDepthBufferLeft()->readOneFrame(fin_depth_l, n) 
	//	||!cViewInterpolation.getDepthBufferRight()->readOneFrame(fin_depth_r, n)
		)
	{
		fprintf(stderr, "Can't read depth frame\n");
		return 3;
	}
	cv::Mat depImg(cParameter.getSourceHeight(), cParameter.getSourceWidth(), CV_8UC1, cViewInterpolation.getDepthBufferLeft()->getBuffer());
	cv::cvtColor(depImg, bgrImg, CV_YUV2BGR_I420);
	cv::imwrite("Origindepth.png", depImg);

	cViewInterpolation.setFrameNumber(n - cParameter.getStartFrame());   //the index the frame

	// 读取视图的一帧
	if (!yuvBuffer.readOneFrame(fin_view_l, n)) return 3;

	cv::Mat yuvImg(cParameter.getSourceHeight() * 3 / 2, cParameter.getSourceWidth(), CV_8UC1, yuvBuffer.getBuffer());
	cv::cvtColor(yuvImg, bgrImg, CV_YUV2BGR_I420);
	cv::imwrite("OriginLeft.png", bgrImg);

	printf("Have load depth and origin left image");

	// 并在CViewInterpolation中载入左右视图数据，用 1, 0 来指定是 left 还是 right
	if (!cViewInterpolation.SetReferenceImage(1, &yuvBuffer))return 3;
	printf(".");

	//if (!yuvBuffer.readOneFrame(fin_view_r, n))return 3;
	//if (!cViewInterpolation.SetReferenceImage(0, &yuvBuffer))return 3;
	//printf(".");

	// 视图合成
	if (!cViewInterpolation.DoViewInterpolation(&yuvBuffer)) return 3;
	printf(".");


	// 将 yuv 文件和bmp文件输出到磁盘上
	if (!yuvBuffer.writeOneFrame(fout,1))  return 3;

	// 将 yuv 文件单帧输出 (参数1 增加bmp图片)


#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	printf("->End (%.4f sec)\n", (double)(finish - start) / CLOCKS_PER_SEC);
	start = finish;
#else
	printf("->End\n");
#endif

	//} // for n

	fclose(fout);
	fclose(fin_view_l);
	fclose(fin_view_r);
	fclose(fin_depth_l);
	fclose(fin_depth_r);

#ifdef OUTPUT_COMPUTATIONAL_TIME
	finish = clock();
	printf("Total: %.4f sec\n", ((double)(finish - first)) / ((double)CLOCKS_PER_SEC));
#endif

	return 0;
}

