/**
	@file	ImageBufferAlloc.cpp
	@date   2021.12.20
	@author Yeo Lip Yoon (magicst3@gmail.com)
	@brief
		ImageBuffer allocation API
	@remark	 	
*/

#include "ImageBufferAlloc.h"
#define C_NRML "\033[0m"
#define C_YLLW "\033[33m"
#define C_AQUA "\033[36m"
#define C_BLUE "\x1b[34m"

#ifdef USE_SHARDMEMORY
#define USE_SHM_LOCK mSharedMemory->Lock();
#define USE_SHM_UNLOCK mSharedMemory->Unlock();
#else
#define USE_SHM_LOCK
#define USE_SHM_UNLOCK
#endif


ImageBufferAlloc::ImageBufferAlloc(): 
    mImageBuffer(NULL),
    mLockBuffer(),
    mMaxSize_per_onebuf(0),
    mDebug_enable(false)
{
    printf("create ImageBufferAlloc\n");
#ifdef USE_SHARDMEMORY
    mSharedMemory = NULL;
#endif

}
ImageBufferAlloc::~ImageBufferAlloc(){
    printf("delete ImageBufferAlloc\n");

    mimgaddr_Idx_refer.clear();
    mimgaddr_List.clear();
   
	for (int row = 0; row < (int)mimgaddr_List_all.size(); ++row) {
		mimgaddr_List_all[row].clear(); 
	}
	mimgaddr_List_all.clear();


#ifdef USE_SHARDMEMORY
    if (mSharedMemory != NULL) {
		int ret;
        ret = mSharedMemory->Destroy();
        printf("mSharedMemory Destory =%d\n", ret);
        ret = mSharedMemory->Close();
        printf("mSharedMemory Close =%d\n", ret);
        delete mSharedMemory;
        mSharedMemory = NULL;

        mImageBuffer = NULL;
    }
#else
    if (mImageBuffer) {
        delete[] mImageBuffer;
        mImageBuffer = NULL;
    }
#endif


}
int ImageBufferAlloc::createImageBuffer(int width, int height, int channel, int numof_imgbuf, int numofslot)
{
    if (width <= 0 && height <= 0 && channel <= 0 && numof_imgbuf <= 0 && numofslot <= 0)
        return -1;
	//buffer clear
	mimgaddr_Idx_refer.clear();
	mimgaddr_List.clear();
	for (int row = 0; row < (int)mimgaddr_List_all.size(); ++row) {
		mimgaddr_List_all[row].clear();
	}
	mimgaddr_List_all.clear();

#ifdef USE_SHARDMEMORY
    if (mSharedMemory != NULL) {
		int ret;
        ret = mSharedMemory->Destroy();		
        printf("mSharedMemory Destory =%d\n", ret);
		ret = mSharedMemory->Close();
        printf("mSharedMemory Close =%d\n", ret);
        delete mSharedMemory;
        mSharedMemory = NULL;

        mImageBuffer = NULL;
    }
    
    mSharedMemory = new OSAC::CSharedMemory;
    int32_t nRet = mSharedMemory->Create("SHM_IMGBUFF", width * height * channel * numof_imgbuf * numofslot);
    printf("Create Result=%d\n", nRet);
    mImageBuffer = reinterpret_cast<unsigned char*>(mSharedMemory->GetAddress());
    printf("sharedMemory Total Size %x %d\n", (mSharedMemory->GetSize()), (mSharedMemory->GetSize()));
#else
    if (mImageBuffer) {
        delete[] mImageBuffer;
        mImageBuffer = NULL;
    }

	//buffer alloc
    mImageBuffer = new unsigned char[width * height * channel * numof_imgbuf * numofslot];
#endif
    mMaxSize_per_onebuf = width * height * channel;
    //printf("->Image Buffer addr start %p\n\n", mImageBuffer);
    printf("%s->Image Buffer addr start %p total_size = 0x%x\n\n", C_BLUE, mImageBuffer, mMaxSize_per_onebuf * numof_imgbuf * numofslot);
    
#if 1
/*
	for (int a = 0; a < numofslot; a++) {
		for (int i = 0; i < numof_imgbuf; i++) {
			imageAddr_idx imgaddr;
			int toffset = i + (a*numof_imgbuf);
			imgaddr.number = toffset;
			imgaddr.used = 0;
			imgaddr.addr = mImageBuffer + (mMaxSize_per_onebuf * (toffset));
			imgaddr.size = width * height * channel;
			printf("number %03d[slot=%d,num=%d], paddr= %p, size=0x%x\n", toffset,a,i, imgaddr.addr, imgaddr.size);

			mimgaddr_Idx_refer.push_back(imgaddr);
			mimgaddr_List.push_back(imgaddr);
		}
	}
*/
	for (int a = 0; a < numofslot; a++) {
		std::list<imageAddr_idx> imgaddr_List_per_slot;
		for (int i = 0; i < numof_imgbuf; i++) {
			imageAddr_idx imgaddr;
			int toffset = i + (a*numof_imgbuf);
			imgaddr.number = toffset;
			imgaddr.used = 0;
			imgaddr.addr = mImageBuffer + (mMaxSize_per_onebuf * (toffset));
			imgaddr.size = width * height * channel;
			printf("number %03d[slot=%d,num=%03d], paddr= %p, size=0x%x\n", toffset,  a, i, imgaddr.addr, imgaddr.size);

			mimgaddr_List.push_back(imgaddr);
			mimgaddr_Idx_refer.push_back(imgaddr);
			imgaddr_List_per_slot.push_back(imgaddr);
		}
		mimgaddr_List_all.push_back(imgaddr_List_per_slot);
		printf("-------------------------------\n");
	}
#else

	for (int i=0; i < numof_imgbuf; i++) {
        imageAddr_idx imgaddr;
        imgaddr.number = i;
        imgaddr.used = 0;
//        imgaddr.addr = &mImageBuffer[width * height * channel * (i * numof_imgbuf)];
        imgaddr.addr = mImageBuffer + (mMaxSize_per_onebuf * (i));
        imgaddr.size = width * height * channel;
        //imgaddr.addr = new unsigned char[imgaddr.size];
        //printf("number %d, paddr= %p, size=%d\n", i, imgaddr.addr, imgaddr.size);
        printf("number %03d, paddr= %p, size=0x%x\n", i, imgaddr.addr, imgaddr.size);

        mimgaddr_Idx_refer.push_back(imgaddr);
        mimgaddr_List.push_back(imgaddr);
    }
#endif
	printf("%s", C_NRML);

    return 1;
}
int ImageBufferAlloc::getAvailIdx_ImageBuffer(void)
{
    int cnt = 0;
    while (!mimgaddr_List.empty())
    {
        imageAddr_idx tbuf;
        tbuf = mimgaddr_List.front();
        if (tbuf.used == 0) {
            std::lock_guard<std::mutex> lock(mLockBuffer);

            //not used
            mimgaddr_List.pop_front();
            mimgaddr_List.push_back(tbuf);
            if(mDebug_enable)
				printf("assigned index=%d", tbuf.number);
            return tbuf.number;
        }
        else if (tbuf.used > 0) {
            std::lock_guard<std::mutex> lock(mLockBuffer);

            //used
            mimgaddr_List.pop_front();
            mimgaddr_List.push_back(tbuf);
        }
        cnt++;
        if(cnt > mimgaddr_List.size()) {
			printf("......  image buffer is full, all used... if this commment is continuous, Please increase buffer size on ImageBuffer\n");
            std::this_thread::sleep_for(std::chrono::microseconds(400000));
        }
    }
    return -1;
}
int ImageBufferAlloc::getAvailIdx_ImageBuffer_slot(int slot)
{
	int cnt = 0;
	while (!mimgaddr_List_all[slot].empty())
	{
		imageAddr_idx tbuf;
		tbuf = mimgaddr_List_all[slot].front();
		if (tbuf.used == 0) {
			std::lock_guard<std::mutex> lock(mLockBuffer);

			//not used
			mimgaddr_List_all[slot].pop_front();
			mimgaddr_List_all[slot].push_back(tbuf);
			if (mDebug_enable)
				printf("assigned index=%d", tbuf.number);
			return tbuf.number;
		}
		else if (tbuf.used > 0 ) {
			std::lock_guard<std::mutex> lock(mLockBuffer);

			//used
			mimgaddr_List_all[slot].pop_front();
			mimgaddr_List_all[slot].push_back(tbuf);
		}
		cnt++;
		if (cnt > mimgaddr_List_all[slot].size()) {
			printf("......  image buffer is full, all used... if this commment is continuous, Please increase buffer size on ImageBuffer\n");
			std::this_thread::sleep_for(std::chrono::microseconds(400000));
		}
	}
	return -1;
}

int ImageBufferAlloc::acquire_ImageBuffer(int idx, unsigned char** addr, int* size)
{
    std::lock_guard<std::mutex> lock(mLockBuffer);
    list<imageAddr_idx>::iterator it;

    for (it = mimgaddr_List.begin(); it != mimgaddr_List.end(); it++) {
        if (it->number == idx) {
            it->used++;
            *addr = it->addr;
            *size = it->size;
            if (mDebug_enable)
                printf("find ok -> used = % d, number = % d, addr = % p\n", it->used, it->number, it->addr);
            return 1;
        }
    }
    return -1;
}
int ImageBufferAlloc::release_ImageBuffer(int idx)
{
    std::lock_guard<std::mutex> lock(mLockBuffer);
    list<imageAddr_idx>::iterator it;

    for (it = mimgaddr_List.begin(); it != mimgaddr_List.end(); it++) {
        if (it->number == idx) {
            it->used--;
            if (it->used < 0) it->used = 0;
            if (mDebug_enable)
                printf("release ok -> used = % d, number = % d, addr = % p\n", it->used, it->number, it->addr);
            return 1;
        }
    }
    return -1;
}

int ImageBufferAlloc::copy_to_imageBuffer(int idx, unsigned char* data, int size)
{
    if (data == NULL || size <= 0)
        return -1;
    std::lock_guard<std::mutex> lock(mLockBuffer);
    list<imageAddr_idx>::iterator it;
    int tsize = size;

    for (it = mimgaddr_List.begin(); it != mimgaddr_List.end(); it++) {
        if (it->number == idx) {
            USE_SHM_LOCK
            if (mMaxSize_per_onebuf < tsize) {
                tsize = mMaxSize_per_onebuf;
				printf("your ImageBuffer size is smaller than ADTF DAT imageData.\n");
            }

            unsigned char* ptr = it->addr;
            memcpy(ptr, data, tsize);

            it->size = tsize;
            if (mDebug_enable)
                printf("copy ok -> used = % d, number = % d, addr = % p, size =%d\n", it->used, it->number, it->addr, it->size);
            USE_SHM_UNLOCK
            return 1;
        }
    }
    return -1;
}

int ImageBufferAlloc::acquire_ImageBuffer_slot(int slot, int idx, unsigned char** addr, int* size)
{
	std::lock_guard<std::mutex> lock(mLockBuffer);
	list<imageAddr_idx>::iterator it;

	for (it = mimgaddr_List_all[slot].begin(); it != mimgaddr_List_all[slot].end(); it++) {
		if (it->number == idx) {
			it->used++;
			*addr = it->addr;
			*size = it->size;
			if (mDebug_enable)
				printf("find ok -> used = % d, number = % d, addr = % p\n", it->used, it->number, it->addr);
			return 1;
		}
	}
	return -1;
}
int ImageBufferAlloc::release_ImageBuffer_slot(int slot,int idx)
{
	std::lock_guard<std::mutex> lock(mLockBuffer);
	list<imageAddr_idx>::iterator it;

	for (it = mimgaddr_List_all[slot].begin(); it != mimgaddr_List_all[slot].end(); it++) {
		if (it->number == idx) {
			it->used--;
			if (it->used < 0) it->used = 0;
			if (mDebug_enable)
				printf("release ok -> used = % d, number = % d, addr = % p\n", it->used, it->number, it->addr);
			return 1;
		}
	}
	return -1;
}

int ImageBufferAlloc::copy_to_imageBuffer_slot(int slot, int idx, unsigned char* data, int size)
{
	if (data == NULL || size <= 0)
		return -1;
	std::lock_guard<std::mutex> lock(mLockBuffer);
	list<imageAddr_idx>::iterator it;
	int tsize = size;

	for (it = mimgaddr_List_all[slot].begin(); it != mimgaddr_List_all[slot].end(); it++) {
		if (it->number == idx) {
			USE_SHM_LOCK
				if (mMaxSize_per_onebuf < tsize) {
					tsize = mMaxSize_per_onebuf;
					printf("your ImageBuffer size is smaller than ADTF DAT imageData.\n");
				}

			unsigned char* ptr = it->addr;
			memcpy(ptr, data, tsize);

			it->size = tsize;
			if (mDebug_enable)
				printf("copy ok -> used = % d, number = % d, addr = % p, size =%d\n", it->used, it->number, it->addr, it->size);
			USE_SHM_UNLOCK
				return 1;
		}
	}
	return -1;
}
void ImageBufferAlloc::info_ImageBuffer()
{
    //std::lock_guard<std::mutex> lock(mLockBuffer);
    list<imageAddr_idx>::iterator it;
	//vector<std::list<imageAddr_idx>>::iterator it2;

    printf("info)\n");
    for (it = mimgaddr_List.begin(); it != mimgaddr_List.end(); it++) {
        printf("used= %d, number = %d, addr = %p, size=%d\n", it->used, it->number, it->addr, it->size);
    }

}

void ImageBufferAlloc::info_ImageBuffer_slot()
{
	//std::lock_guard<std::mutex> lock(mLockBuffer);
	list<imageAddr_idx>::iterator it;
	//vector<std::list<imageAddr_idx>>::iterator it2;

	printf("info)] -> slot\n");
	for (int i = 0; i < mimgaddr_List_all.size(); i++) {
		for (it = mimgaddr_List_all[i].begin(); it != mimgaddr_List_all[i].end(); it++) {
			printf("used= %d, number = %d, addr = %p, size=%d\n", it->used, it->number, it->addr, it->size);
		}
	}
	printf("-------------------------------\n");

}

