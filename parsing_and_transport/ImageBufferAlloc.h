/**
	@file	ImageBufferAlloc.h
	@date   2021.12.20
	@author Yeo Lip Yoon (magicst3@gmail.com)
	@brief
		ImageBuffer allocation API
	@remark	 	
*/

#ifndef IMAGE_BUFFER_ALLOC
#define IMAGE_BUFFER_ALLOC

//#define USE_SHARDMEMORY

#ifdef USE_SHARDMEMORY
#include "osal/SharedMemory.h"
#include "osac/CSharedMemory.h"
#endif

#include <list>
#include <stdio.h>
#include<mutex>
#include<vector>

using namespace std;

typedef struct {
	//int slot = 0;
    int number = 0;
    int used = 0;
    int size = 0;
    unsigned char* addr;
}imageAddr_idx;

class ImageBufferAlloc
{
public:
    ImageBufferAlloc();
    ~ImageBufferAlloc();

    int createImageBuffer(int width, int height, int channel, int numofbuf = 30, int numofslot = 1);

	int getAvailIdx_ImageBuffer(void);
	int acquire_ImageBuffer(int idx, unsigned char** addr, int* size);
    int release_ImageBuffer(int idx);
    int copy_to_imageBuffer(int idx, unsigned char* data, int size);

	int getAvailIdx_ImageBuffer_slot(int slot);
	int acquire_ImageBuffer_slot(int slot,int idx, unsigned char** addr, int* size);
	int release_ImageBuffer_slot(int slot,int idx);
	int copy_to_imageBuffer_slot(int slot,int idx, unsigned char* data, int size);

    void info_ImageBuffer();
    void info_ImageBuffer_slot();

    //public:

private:
    vector<imageAddr_idx> mimgaddr_Idx_refer;
	//vector<vector<imageAddr_idx>> mimgaddr_Idx_refer;

	std::mutex mLockBuffer;
    std::list<imageAddr_idx> mimgaddr_List;
	vector<std::list<imageAddr_idx>> mimgaddr_List_all;
	int mMaxSize_per_onebuf;
    unsigned char* mImageBuffer;
    bool mDebug_enable;

    #ifdef USE_SHARDMEMORY
    OSAC::CSharedMemory *mSharedMemory;
    #endif
};

#endif IMAGE_BUFFER_ALLOC