#ifndef ADTF_STREAMING_LIB_H
#define ADTF_STREAMING_LIB_H

#include <adtf_streaming.h>

#include <string>
#include <map>
#include <set>
#include <vector>


typedef struct {
    int  nStreamId;
    std::string name;
    std::string type;
    int  nType;
    int  nMajorType;
    int  nSubType;
    int  nBlockCount;
    int  nChunkSize;

	int nimgWidth;
	int nimgHeight;
	int nimgBitPerPixel;
    //tTimeStamp startTime;
    long long lduration;
	
} sStreamInfo;

class ADTFStreamingLib
{
public:
    ADTFStreamingLib();
    ~ADTFStreamingLib();

    int Decompress(const tChar* strFilename, tChar* strOutputFile);

    /* Analysis DAT file Infomation in detail*/
    int AccessFileData(const tChar* strFilename, const tChar* strCSVFilePath);
    
    /* Get File Infomation*/
    int QueryFileInfo(const tChar* strFilename, std::map<std::string,std::string> &header, std::vector<sStreamInfo> &streams);
    
    /* Make user DAT file from original DAT */
    int CopyChunks(const char* strFilename, const char* targetFilename, std::vector<sStreamInfo> streams, float percentage=100.0);
};

#endif //ADTF_STREAMING_LIB_H

