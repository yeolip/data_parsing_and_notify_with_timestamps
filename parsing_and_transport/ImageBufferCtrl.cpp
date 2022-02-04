/**
	@file	ImageBufferCtrl.cpp
	@date   2021.12.20
	@author Yeo Lip Yoon (magicst3@gmail.com)
	@brief
		ImageBuffer Control API
	@remark	 	
*/

#ifdef WIN32
#include <windows.h>
#endif

#ifdef OPENCV_ON 
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#ifdef OPENCV_ON_WITH_OPENCL
#include "opencv2/core/ocl.hpp"
#endif
#endif

#include "ImageBufferCtrl.h"

#define WORKING_FILE_PATH_SIZE 256

#define C_NRML "\033[0m"
#define C_PRPL "\033[35m"
#define C_RED  "\033[31m"
#define C_CYAN "\x1b[36m"
#define C_YLLW "\033[33m"

#define SDATSEND_PARSING_I_Print(...)	\
	{							\
		printf(C_PRPL);	\
		printf(__VA_ARGS__);	\
		printf(C_NRML);	\
	}

#define SDATSEND_SEND_I_Print(...)	\
	{							\
		printf(C_CYAN);	\
		printf(__VA_ARGS__);	\
		printf(C_NRML);	\
    }

#define SDATSEND_RECV_I_Print(...)	\
	{							\
		printf(C_YLLW);	\
		printf(__VA_ARGS__);	\
		printf(C_NRML);	\
    }


ImageBufferCtrl::ImageBufferCtrl() :
    mRunning(false),
    mSleeptime(33333),
    mFPS(30),
    mAvail_DATVideoCount(0),
    mThread(),
    mThreadDisp(),
    mLock(),
    //mImageBuffer(0),
    mDebug_enable(true)
{
}
ImageBufferCtrl::~ImageBufferCtrl()
{
    while (!mPQ_ImgBuf.empty()){
        mPQ_ImgBuf.pop();
    }
    while (!mDisplayBuffer.empty()){
        mDisplayBuffer.pop_front();
    }
}
int ImageBufferCtrl::setCalculateFPS(long long duration, long long totalofimage)
{
    if (totalofimage < 0)
        return -1;
    mSleeptime = (duration / totalofimage);
    mFPS = (1000000 / mSleeptime);
    cout << "FPS="<< mFPS << endl;
    return 0;
}
int ImageBufferCtrl::setFPS(long long fps)
{
    if (fps <= 0)
        return -1;
    mFPS = fps;
    mSleeptime = (1000000 / fps);
    cout << "FPS=" << mFPS << endl;
    return 0;
}
std::vector<sStreamInfo> ImageBufferCtrl::collectVideoStream_and_Set(std::vector<sStreamInfo> input) {
    std::vector<sStreamInfo> output;
    int max_vidWidth = 0;
    int max_vidHeight = 0;
    int max_vidBitPerPixel = 0;

    for (vector<sStreamInfo>::iterator it = input.begin(); it != input.end(); it++)
    {
        if (it->type == "Video") {			
			if (it->name.compare(0,5,"Image") == 0) { 
	            output.push_back(*it);
				mAvail_DATVideoCount++;
	            if (it->nimgWidth > max_vidWidth)
	                max_vidWidth = it->nimgWidth;
	            if (it->nimgHeight > max_vidHeight)
	                max_vidHeight = it->nimgHeight;
	            if (it->nimgBitPerPixel > max_vidBitPerPixel)
	                max_vidBitPerPixel = it->nimgBitPerPixel;
			}
        }
        //if (it->type == "Structured Data")
        //    output.push_back(*it);
    }
    if (output.size() > 0) {
        std::sort(output.begin(), output.end(), sStreamInfo_vid());
        this->setCalculateFPS(output[0].lduration, output[0].nBlockCount);
        if(mFPS == 0) this->createImageBuffer(max_vidWidth, max_vidHeight, int(max_vidBitPerPixel/8));
        //make buffer size => FPS * number of Image's slot 
        else this->createImageBuffer(max_vidWidth, max_vidHeight, int(max_vidBitPerPixel / 8), mFPS, output.size());
    }   
    return output;
}

int ImageBufferCtrl::start() {
    std::lock_guard<std::mutex> lock(mLock);
	printf("start\n");
    if (mRunning == false){

        mThread = std::thread(&ImageBufferCtrl::thread_OnProcess, this); 
        mThreadDisp = std::thread(&ImageBufferCtrl::thread_DisplayOnProcess, this);
        mRunning = true;
    }
    return 0;
}
int ImageBufferCtrl::stop() {
    std::lock_guard<std::mutex> lock(mLock);
	printf("stop\n");
    if (mRunning)
    {
        mRunning = false;

        mThread.join();

        mThreadDisp.join();
#ifdef OPENCV_ON 
        cv::destroyAllWindows();
#endif


    }
    
    return 0;
}
int ImageBufferCtrl::wait_and_stop() {
	while (!mPQ_ImgBuf.empty()){	
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
    while (!mDisplayBuffer.empty()) {
        mDisplayBuffer.pop_front();
    }
	this->stop();
    return 0;
}


int ImageBufferCtrl::parserRawData(const char* strFilename, std::vector<sStreamInfo> vinfo, const char* strOutFilePath)
{
    if (this->getAvailIdx_ImageBuffer() == -1) {
        printf("ERROR - Buffer is not allocated. Please call createImageBuffer, first...........!!!!");
        return -1;
    }
    // open the dat file
    IADTFFileReader* pFileReader = IADTFFileReader::Create();
    tResult result;

    if (IS_FAILED(result = pFileReader->Open(strFilename))) {
        //cerr << "unable to open file " << strFilename << endl;
        IADTFFileReader::Release(pFileReader);
        return(result);
    }

    // get the stream description
    const tADTFStreamDescriptor* pStreamDescriptor = pFileReader->GetStreamDescriptor(vinfo[0].nStreamId);
    if (!pStreamDescriptor) {
        cerr << "unable to get stream info" << endl;
        IADTFFileReader::Release(pFileReader);
        return(0);
    }

    bool bCompressed = (pStreamDescriptor->nType == tADTFStreamDescriptor::SD_COMPRESSED_VIDEO);
    //IStreamingCodec* pCodec = NULL;
    tBitmapFormat sOutputFormat;

    if (bCompressed) {
        string strCodec = pStreamDescriptor->uData.sCompressedVideoFormat.strCodec;
        cout << "found codec '" << strCodec << "'" << endl;
    }
    else {
        memcpy(&sOutputFormat, &pStreamDescriptor->uData.sVideoFormat.sBitmapFormat, sizeof(sOutputFormat));
    }

    int image_width = sOutputFormat.nWidth;
    int image_height = sOutputFormat.nHeight;
    int image_channel = sOutputFormat.nBitsPerPixel / 8;

    cADTFDataBlock* pDataBlock = NULL;
    tResult nRet = ERR_NOERROR;
    unsigned char* pOutputData = NULL;
    //pOutputData = new unsigned char[1280*720*2];


    std::chrono::time_point<chrono::system_clock>  startT = std::chrono::system_clock::now();
    std::chrono::time_point<chrono::system_clock>  curT;
    tTimeStamp procTimePerVidStream = 0;
    bool availableTime = false;

	long long pcnt = 0;;
    tTimeStamp startTime = 0;
    int blockTotalcount = pFileReader->GetDataBlockCount();
    tTimeStamp fDuration = pFileReader->GetTimeDuration();
    for (int i = 0; i < blockTotalcount; i++) {
        nRet = pFileReader->Read(&pDataBlock);
        if (IS_OK(nRet)) {
            if (i == 0) {
                startTime = pDataBlock->GetTime();
            }
            for (vector<sStreamInfo>::iterator it = vinfo.begin(); it != vinfo.end(); it++)
            {
                if (it->nStreamId == pDataBlock->GetStreamId()) {
                    const void* pData;
                    datFrameBuffer_t tFrame;

                    //tTimeStamp a = pDataBlock->GetTime();
                    //tTimeStamp b = pDataBlock->GetFileTime();
                    //int c = pDataBlock->GetBlockPos();
                    //int d = pDataBlock->GetFileBlockPos();
                    //int e = pDataBlock->GetStreamId();

                    // get data
                    tUInt64 nSize = pDataBlock->GetData(&pData);
					
                    unsigned char* taddr = NULL;
                    int tsize = 0;
                    tFrame.data = 0;

                    int tslot = atoi(it->name.substr(5, it->name.length()).c_str());
                    int tidx = this->getAvailIdx_ImageBuffer_slot(tslot);

                    mLock.lock();
                    this->acquire_ImageBuffer_slot(tslot, tidx, &taddr, &tsize);
                    tFrame.lock_idx = tidx;

                    // make packet
                    tFrame.name = it->name;
                    tFrame.type = it->type;
                    tTimeStamp ttime = pDataBlock->GetTime();
                    if (ttime <= 0) tFrame.time = 0;
                    else tFrame.time = ttime - startTime + (mSleeptime* mFPS); // add  rest 2 frame time 

                    tFrame.width = it->nimgWidth;
                    tFrame.height = it->nimgHeight;
                    tFrame.channel = (it->nimgBitPerPixel / 8);
                    tFrame.data = taddr;
                    if (0) {
                        //pOutputData = new unsigned char[nSize];
                        //memcpy(pOutputData, pData, nSize);
                        //pData = pOutputData;
                    }
                    else {
                        this->copy_to_imageBuffer_slot(tslot, tidx, (unsigned char*)pData, nSize);
                    }
					
                    //mLock.lock();
                    if (it->nStreamId == vinfo[vinfo.size()-1].nStreamId) {
                        availableTime = true;
                        curT = std::chrono::system_clock::now();
                        procTimePerVidStream = std::chrono::duration_cast<std::chrono::microseconds>(curT - startT).count();
                        if(mDebug_enable)
                            SDATSEND_PARSING_I_Print("streamID=%02d , idx= %d , %s, time=%ld, parsingDuration=%ld\n", it->nStreamId, pcnt, tFrame.name.c_str(), tFrame.time, procTimePerVidStream);
                            //cout << "streamID=" << it->nStreamId << ", idx=" << pcnt << " , time=" << tFrame.time << " ,parsingDuration=" << procTimePerVidStream << endl;
                        startT = curT;
                    }else{
                        availableTime = false;
                        if (mDebug_enable)
                            SDATSEND_PARSING_I_Print("streamID=%02d , idx= %d , %s, time=%ld\n", it->nStreamId, pcnt, tFrame.name.c_str(), tFrame.time);
                        //cout << "streamID=" << it->nStreamId << ", idx=" << pcnt << " , time=" << tFrame.time << "," << tFrame.name<< endl;
                    }
                    mPQ_ImgBuf.push(tFrame);
                    mLock.unlock();
                    //this->release_ImageBuffer_slot(tslot, tidx);
					
                    
                    if (0) {
                        if (pOutputData) {
                            delete[] pOutputData;
                            pOutputData = NULL;
                        }
                    }
                    // send packet
					pcnt++;
                }
            }
        }
///*
        // parsing시에 빠르게 진행될수도 있고, 느릴수도 있음. 속도를 판별
        #ifdef WIN32
            timeBeginPeriod(1);
        #endif
        if(availableTime == true){
            //long long intervalTime = (tTimeStamp)(mSleeptime / vinfo.size() - 1) - procTimePerVidStream;
            long long intervalTime = (tTimeStamp)(mSleeptime * 0.7) - procTimePerVidStream;
            if ((intervalTime) > 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(intervalTime));
                if (mDebug_enable)
                    //cout << " -->parse_delay=" << intervalTime << ",pproc_delay=" << procTimePerVidStream << endl;
                    SDATSEND_PARSING_I_Print(" -->parse_delay=%ld ,pproc_delay=%ld\n", intervalTime, procTimePerVidStream);

                //availableTime == false;
            }
            else {
                std::this_thread::sleep_for(std::chrono::microseconds(1000));
            }
            availableTime = false;
        }
        //else {
        //    std::this_thread::sleep_for(std::chrono::microseconds(1000));
        //}
        #ifdef WIN32
            timeEndPeriod(1);
        #endif

//*/


        
    }

    // close file
    pFileReader->Close();
    IADTFFileReader::Release(pFileReader);
    return(nRet);
}

void ImageBufferCtrl::thread_OnProcess() {
    bool bAvailableInterval = false;
    tTimeStamp timeInterval = 0;
    long long scnt;
    long long fcnt;
    string strPrename = "saveRawImg";

	while (!mRunning)
	    std::this_thread::sleep_for(std::chrono::seconds(1));
	cout<<"Start thread_OnProcess"<<endl;

    scnt = 0;
    fcnt = 0;
    std::chrono::time_point<chrono::system_clock>  startT = std::chrono::system_clock::now();
    //std::chrono::steady_clock
    while (mRunning) {
		//mRunning
        bAvailableInterval = false;
        std::chrono::time_point<chrono::system_clock>  curT = std::chrono::system_clock::now();

        tTimeStamp microSec = std::chrono::duration_cast<std::chrono::microseconds>(curT - startT).count();
        while (!mPQ_ImgBuf.empty())
        {
            datFrameBuffer_t tFrame;
            tFrame = mPQ_ImgBuf.top();
            if (microSec >= tFrame.time) {
                
                //publish 
                mLock.lock();
                if (mDebug_enable)
                    SDATSEND_SEND_I_Print("%d | %d , offsetTime=%ld , time=%ld, %s, stack[%d]\n", scnt, fcnt, (tTimeStamp)microSec, tFrame.time, tFrame.name.c_str(),mPQ_ImgBuf.size());
                //cout << scnt << " |" << fcnt << " offsetTime=" << microSec << ",time=" << tFrame.time << ", " << tFrame.name << endl; // << ", " << tFrame.width << ", " << tFrame.height << ", " << tFrame.channel << endl;
				int tsize = tFrame.width * tFrame.height * tFrame.channel;
							
				mPQ_ImgBuf.pop();
                //mLock.unlock();
                if(tFrame.type == "Video"){
                    int tslot = atoi(tFrame.name.substr(5, tFrame.name.length()).c_str());
					//skip comparing tFrame.name about "Image"
					//cout<< atoi(tFrame.name.substr(5, tFrame.name.length()).c_str()) << " ====>" <<endl;
					//this->publish_ImgFrm(tFrame.data,tsize, atoi(tFrame.name.substr(5, tFrame.name.length()).c_str()), tFrame.width, tFrame.height, tFrame.channel);
                    //if(tslot == 0)
                    mDisplayBuffer.push_back(tFrame);
                    mLock.unlock();
                    this->release_ImageBuffer_slot(tslot, tFrame.lock_idx);
                }
                else {
                    mLock.unlock();
                }

                if (0) {
                    ofstream oFile;
                    std::string strCnt = std::to_string(fcnt);
                    std::string strFilename = strPrename + '_' + tFrame.name + '_' + strCnt;
                    oFile.open(strFilename.c_str(), ofstream::binary);
                    oFile.write((char*)tFrame.data, tFrame.width * tFrame.height * tFrame.channel);
                    oFile.close();
                }
            }
            else {
    
                bAvailableInterval = true;
                timeInterval = tFrame.time - microSec + 2000;
                break;
            }
            fcnt++;
            //printf("fcnt=%d\n", fcnt);
        }
        scnt++;
        std::chrono::time_point<chrono::system_clock>  curT2 = std::chrono::system_clock::now();

        tTimeStamp processTime = std::chrono::duration_cast<std::chrono::microseconds>(curT2 - curT).count();

        long long intervalTime = ((tTimeStamp)(mSleeptime/3)) - processTime;
        #ifdef WIN32
            timeBeginPeriod(1);
        #endif
        if (bAvailableInterval == true) {
            std::this_thread::sleep_for(std::chrono::microseconds(timeInterval));
            if (mDebug_enable)
                SDATSEND_SEND_I_Print(" ->t_delay_flag=%ld ,proc_delay=%ld\n", intervalTime, processTime);
            //cout << " ->t_delay_flag=" << timeInterval << ",proc_delay=" << processTime << endl;
        }else if(intervalTime > 0 && processTime > 1000) {
            std::this_thread::sleep_for(std::chrono::microseconds(intervalTime));
            if (mDebug_enable)
                SDATSEND_SEND_I_Print(" ->t_delay    =%ld ,proc_delay=%ld\n", intervalTime, processTime);
            //cout << " ->t_delay=" << intervalTime << ",proc_delay=" << processTime << endl;
        }
        else{
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        //    cout << " ->t_delay= 1000 ,proc_delay=" << processTime << endl;
        }
        #ifdef WIN32
            timeEndPeriod(1);
        #endif

    }
}

void ImageBufferCtrl::thread_DisplayOnProcess() {
    while (!mRunning)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "Start thread_DisplayOnProcess" << endl;

#ifdef OPENCV_ON
    //printf("start%d\n", cv::startWindowThread());
    ///*
    for (int c = 0; c < mAvail_DATVideoCount; c++) {
        std::string strDisplayName = "DisplayWindow_" + std::to_string(c);
        //cv::destroyWindow(strDisplayName);
        //cv::startWindowThread();
        //cv::getWindowProperty(strDisplayName, c);
        cv::namedWindow(strDisplayName, cv::WINDOW_AUTOSIZE);
        printf("open display windows=%d\n", c);
    }
    //*/
#endif

    while (mRunning) {
        //mRunning



        while (!mDisplayBuffer.empty())
        {
            printf("Disp %d\n", mDisplayBuffer.size());
            mLock.lock();
            datFrameBuffer_t tFrame;
            tFrame = mDisplayBuffer.front();

            mDisplayBuffer.pop_front();

#ifdef OPENCV_ON 
            int theight, twidth, ttype;
            int tslot;
            
            theight = tFrame.height;
            twidth = tFrame.width;
            ttype = (tFrame.channel * 8) - 8;
            tslot = atoi(tFrame.name.substr(5, tFrame.name.length()).c_str());
            std::string strDisplayName = "DisplayWindow_" + std::to_string(tslot);
            //printf("%s\n", strDisplayName.c_str());

#ifdef OPENCV_ON_WITH_OPENCL
            cv::UMat buf, buf_resize;
            const cv::Mat orgimg2(theight, twidth, ttype, tFrame.data);
            cv::UMat orgimg = orgimg2.getUMat(cv::ACCESS_READ);
#else
            cv::Mat buf, buf_resize;
            const cv::Mat orgimg(theight, twidth, ttype, tFrame.data);
#endif
            if (tFrame.channel == 1) {
                cv::resize(orgimg, buf_resize, cv::Size((int)(twidth / 2), (int)(theight / 2)), 0.0, 0.0, 1);
            }
            else if (tFrame.channel == 2) {
                cv::cvtColor(orgimg, buf, cv::COLOR_YUV2BGR_YUYV);
                //cv::cvtColor(orgimg, buf, cv::COLOR_YUV2GRAY_YUYV);
                cv::resize(buf, buf_resize, cv::Size((int)(twidth / 2), (int)(theight / 2)), 0.0, 0.0, 1);
                //cv::cvtColor(buf_resize, buf_resize, cv::COLOR_BGR2GRAY);
                //cv::resize(orgimg, buf_resize, cv::Size((int)(twidth / 1), (int)(theight / 1)), 0.0, 0.0, 1);
                //cv::cvtColor(orgimg, buf, cv::COLOR_YUV2BGR_YUYV);
            }
            else {
                orgimg.copyTo(buf_resize);
            }
            //this->release_ImageBuffer_slot(tslot, tFrame.lock_idx);

            cv::imshow(strDisplayName, buf_resize);
            cv::waitKey(1);
            mLock.unlock();


#endif


        }
#ifdef WIN32
//        timeBeginPeriod(1);
#endif
        //std::this_thread::sleep_for(std::chrono::microseconds(1000));
#ifdef WIN32
//        timeEndPeriod(1);
#endif
    }

    cout << "End display!!" << endl;

}



int ImageBufferCtrl::autoPlay(const char* fileName){
    std::map<std::string, std::string> tHeader;
    std::vector<sStreamInfo> tStreams;
    std::vector<sStreamInfo> vidStreams;
    
    int nRet;
    char strWorkingFile[WORKING_FILE_PATH_SIZE];

	printf("\nautoPlay file=%s %s\n", fileName , __FUNCTION__);
	
    this->Decompress(fileName, strWorkingFile);
    nRet = this->QueryFileInfo(fileName, tHeader, tStreams);
    printf("\nnRet=%d \n\n", nRet);
	if(nRet<0){
        SDATSEND_SEND_I_Print("\n File is not load....%s please check file path on xml!!!!! \n", fileName);
        SDATSEND_SEND_I_Print("\n File is not load.... please check file path on xml!!!!! \n");
        SDATSEND_SEND_I_Print("\n File is not load.... please check file path on xml!!!!! \n\n");
	}else{	
        SDATSEND_SEND_I_Print("\n File is load....%s OK!!!!! \n",fileName);
        SDATSEND_SEND_I_Print("\n File is load.... OK!!!!! \n");
        SDATSEND_SEND_I_Print("\n File is load.... OK!!!!! \n");
		
        vidStreams = this->collectVideoStream_and_Set(tStreams);
		
        SDATSEND_SEND_I_Print("start");
		this->setDebugEnable(mDebug_enable);
		this->start(); 

		this->parserRawData(fileName, vidStreams, "raw_img_%F_%T_%D");
		//cout<<"wait and stop"<<endl;
        SDATSEND_SEND_I_Print("wait and stop");
		this->wait_and_stop();
		//std::this_thread::sleep_for(std::chrono::seconds(20));
	}
	
    tHeader.clear();
    tStreams.clear();
    vidStreams.clear();

	return 1;
}

int ImageBufferCtrl::enableOpenCL() {
    SDATSEND_SEND_I_Print("\n%s %d\n", __FUNCTION__, __LINE__);
#ifdef OPENCV_ON_WITH_OPENCL
    // Testing whether or not to use OpenCL 
    if (!cv::ocl::haveOpenCL()) {
        std::cout << " Error : this system is not use OpenCL." << std::endl;
        return  -1;
    }

    // Create context
    cv::ocl::Context context;
    if (!context.create(cv::ocl::Device::TYPE_GPU)) {
        std::cout << " Error : it can not create context." << std::endl;
        return  -1;
    }

    // GPU device info
    std::cout << context.ndevices() << " GPU device (s) detected " << std::endl;
    for (size_t i = 0; i < context.ndevices(); i++) {
        cv::ocl::Device device = context.device(i);
        std::cout << " - Device " << i << " --- " << std::endl;
        std::cout << " Name : " << device.name() << std::endl;
        std::cout << " Availability : " << device.available() << std::endl;
        std::cout << "Image Support : " << device.imageSupport() << std::endl;
        std::cout << " OpenCL C version : " << device.OpenCL_C_Version() << std::endl;
    }

    // use device #0 
    cv::ocl::Device(context.device(0));

    // enable OpenCL
    cv::ocl::setUseOpenCL(true);
#endif
    return 0;

}
