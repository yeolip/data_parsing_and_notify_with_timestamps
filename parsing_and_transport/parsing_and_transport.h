#ifndef PARSING_AND_TRANSPORT_H
#define PARSING_AND_TRANSPORT_H

#include <string>
//#include <queue>
#include <list>
#include <stdio.h>
//#include <iostream>
//#include <sstream>
//#include <fstream>
#include <map>
#include <algorithm>

#include<thread>
#include<chrono>
#include<mutex>
#include<vector>

using namespace std;

typedef struct {
    std::string name;
    std::string type;
    tTimeStamp time;
    int width;
    int height;
    int channel;
    unsigned char* data;
    //int lock_idx;
}datFrameBuffer_t;



struct queue_compare2 {
    bool operator()(datFrameBuffer_t a, datFrameBuffer_t b) {
        return a.time > b.time;
    }
};


typedef struct {
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

    int createImageBuffer(int width, int height, int channel, int numofbuf = 10);
    int getAvailIdx_ImageBuffer(void);
    int acquire_ImageBuffer(int idx, unsigned char** addr, int* size);
    int release_ImageBuffer(int idx);
    int copy_to_imageBuffer(int idx, unsigned char* data, int size);
    void info_ImageBuffer();
    //public:

private:
    vector<imageAddr_idx> mimgaddr_Idx_refer;
    std::mutex mLockBuffer;
    std::list<imageAddr_idx> mimgaddr_List;
    int mMaxSize_per_onebuf;
    unsigned char* mImageBuffer;
    bool debug_enable;

};

#endif PARSING_AND_TRANSPORT_H