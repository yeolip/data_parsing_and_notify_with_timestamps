/**
	@file	ImageBufferCtrl.h
	@date   2021.12.20
	@author Yeo Lip Yoon (magicst3@gmail.com)
	@brief
		ImageBuffer Control API
	@remark	 	
*/

#ifndef IMAGE_BUFFER_CTRL
#define IMAGE_BUFFER_CTRL

#include <queue>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include <stdio.h>
#include<thread>
#include<chrono>
#include<mutex>
#include<vector>

#include "ImageBufferAlloc.h"
#include "adtf/ADTFStreamingLib.h"


using namespace std;
using namespace adtfstreaming;


typedef struct {
    std::string name;
    std::string type;
    tTimeStamp time;
    int width;
    int height;
    int channel;
    unsigned char* data;
    int lock_idx;
}datFrameBuffer_t;

struct queue_compare2 {
    bool operator()(datFrameBuffer_t a, datFrameBuffer_t b) {
        return a.time > b.time;
    }
};

struct sStreamInfo_vid {
    bool operator ()(sStreamInfo const& a, sStreamInfo const& b) const {
		//if((strnicmp(a.name.c_str(), "Image", 5) == 0) && (strnicmp(b.name.c_str(), "Image", 5) != 0)) return true;
        if((a.name.compare(0,5,"Image") == 0) && (b.name.compare(0,5,"Image") != 0)) return true;
        if((a.name.compare(0,5,"Image") != 0) && (b.name.compare(0,5,"Image") == 0)) return false;
        //if((strnicmp(a.name.c_str(), "Image", 5) != 0) && (strnicmp(b.name.c_str(), "Image", 5) == 0)) return false;
        if (a.name < b.name) return true;
        if (a.name >= b.name) return false;
    }
};
/**
*	@class		CDATSendServiceWorker
*	@brief		Reading and publishing image frames/lines to other services
*	@details	DAT Camera : Read image chunks and publish image frames to other services
*	@see		IWork
*	@since		1.0
*/
class ImageBufferCtrl : public ImageBufferAlloc, public ADTFStreamingLib
{
    public:
        ImageBufferCtrl();
        ~ImageBufferCtrl();

        int enableOpenCL();

        void setDebugEnable(int flag) { mDebug_enable = flag; }
        int setCalculateFPS(long long duration, long long totalofimage);
        int setFPS(long long fps);
        std::vector<sStreamInfo> collectVideoStream_and_Set(std::vector<sStreamInfo> input);
        int parserRawData(const char* strFilename, std::vector<sStreamInfo> vinfo, const char* strOutFilePath=NULL); 

        //unsigned char* connectImageBuffer(int idx);
        //void disconnectImageBuffer(int idx);
        void thread_OnProcess();
        void thread_DisplayOnProcess();
        int start();
        int stop();
		int wait_and_stop();

        int autoPlay(const char* fileName);

    private:
        priority_queue<datFrameBuffer_t, vector<datFrameBuffer_t>, queue_compare2 > mPQ_ImgBuf;
        tTimeStamp  mBaseTime;
        bool mRunning;
        long long mSleeptime;  //sleeptime per frames    unit: microsecond
        std::mutex mLock;
        std::thread mThread;
        long long mFPS;
        //unsigned char* mImageBuffer;
        //ImageBufferAlloc mimgAlloc;    //        imgAlloc.createImageBuffer(1280, 720, 3, 10);
		int mAvail_DATVideoCount;
        bool mDebug_enable;

        std::list<datFrameBuffer_t> mDisplayBuffer;
        std::thread mThreadDisp;
};

#endif IMAGE_BUFFER_CTRL