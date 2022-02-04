/**
 *
 * Gathering ADTF examples using ADTF library
 *
 * @file
 * Copyright &copy; Audi Electronics Venture GmbH. All rights reserved.
 *
 * @author               $Author: Audi Electronics $
 * @date                 $Date: 2009-07-16 15:36:37 +0200 (Do, 16 Jul 2009) $
 *
 * @remarks
 *
 */


//#include "../stdafx.h"
#include "ADTFStreamingLib.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
using namespace adtfstreaming;

//-----------------------------------------------------------------------------------------------------------------------------

#define     WORKING_FILE_PATH_SIZE              256
#define     Additional_DeviceOriginalTime       0
#define     Additional_Counter                  1

#ifdef __linux
#define strcpy_s(dest, size, source) strcpy(dest, source)
#endif

FILE* __fopen(const char* strFileName, const char* mode)
{
#ifdef WIN32
    FILE* pFileHandle = NULL;
    if (0 == fopen_s(&pFileHandle, strFileName, mode)) {
        return pFileHandle;
    }
#else
    return fopen(strFileName, mode);
#endif
    return NULL;
}

class cLastTimes
{
    typedef map<tUInt16, tTimeStamp> tLastTimes;

public:
    cLastTimes() {}
    ~cLastTimes() {}
    tTimeStamp GetDiffToLastChunkTime(const tUInt16& ui16Stream, const tTimeStamp& tmCurrentTime)
    {
        return GetLastTimeStamp(m_mapLastChunkTime, ui16Stream, tmCurrentTime);
    }
    tTimeStamp GetDiffToLastStreamTime(const tUInt16& ui16Stream, const tTimeStamp& tmCurrentTime)
    {
        return GetLastTimeStamp(m_mapLastStreamTime, ui16Stream, tmCurrentTime);
    }
private:
    tTimeStamp GetLastTimeStamp(tLastTimes& mapLastTimes,
        const tUInt16& ui16Stream,
        const tTimeStamp& tmCurrentTime)
    {
        tTimeStamp tmResult = -1;
        tLastTimes::iterator it = mapLastTimes.find(ui16Stream);
        if (it != mapLastTimes.end())
        {
            tmResult = tmCurrentTime - it->second;
            it->second = tmCurrentTime;
        }
        else
        {
            mapLastTimes[ui16Stream] = tmCurrentTime;
        }
        return tmResult;
    }
    tLastTimes m_mapLastChunkTime;
    tLastTimes m_mapLastStreamTime;
};

//-----------------------------------------------------------------------------------------------------------------------------

ADTFStreamingLib::ADTFStreamingLib(){
    printf("create ADTFStreamingLib\n");

}
ADTFStreamingLib::~ADTFStreamingLib() {
    printf("delete ADTFStreamingLib\n");
}

int ADTFStreamingLib::Decompress(const char* strFilename, char* strOutputFile)
{
    std::string strWork = strFilename;
    int nResult = 0;

    // '.bz2'
    if (strWork.length() < 5) {
        strcpy_s((char *)strOutputFile, WORKING_FILE_PATH_SIZE, strWork.c_str());
        return nResult;
    }
    std::string strExtension = strWork.substr(strWork.length() - 4, 4);
    if (0 == _stricmp(strExtension.c_str(), ".bz2")) {
        // file ends with '.bz2'
        strWork = strWork.substr(0, strWork.length() - 4);
        nResult = IADTFFileCompression::Decompress(strFilename, strWork.c_str(), IADTFFileCompression::CA_Bzip2);
    }

    strcpy_s((char *)strOutputFile, WORKING_FILE_PATH_SIZE, strWork.c_str());
    return nResult;
}

int ADTFStreamingLib::AccessFileData(const tChar* strFilename, const tChar* strCSVFilePath)
{
    IADTFFileReader* pFileReader = IADTFFileReader::Create();
    cLastTimes oRememberTimes;
    if (IS_FAILED(pFileReader->Open(strFilename)))
    {
        IADTFFileReader::Release(pFileReader);
        RETURN_ERROR(ERR_FILE_NOT_FOUND);
    }

    cout << endl << "File data" << endl;
    cout << "------------------------------------------------------------------------------" << endl;


    FILE* hCSVFile = __fopen(strCSVFilePath, "w+");
    if (NULL == hCSVFile)
    {
        cout << endl << "Please close the csv file!" << endl;
        RETURN_ERROR(ERR_NOACCESS);
    }

    // set the labels
    fprintf(hCSVFile, "%s", "stream,stream_name,chunk_time,mediasample_time,device_time,chunk_time_delta_to_lastofstream,media_time_delta_to_lastofstream\n");

    cADTFDataBlock* pDataBlock = NULL;
    tResult nRet = ERR_NOERROR;
    while (nRet == ERR_NOERROR)
    {
        nRet = pFileReader->Read(&pDataBlock);
        if (IS_OK(nRet))
        {
            fprintf(hCSVFile, "%d,%s,%lld,%lld,%lld,%lld,%lld\n",
                pDataBlock->GetStreamId(),
                pFileReader->GetStreamName(pDataBlock->GetStreamId()),
                (long long int) pDataBlock->GetFileTime(),
                (long long int) pDataBlock->GetTime(),
                (long long int) pDataBlock->GetAdditionalTimestamp(Additional_DeviceOriginalTime, -1),
                (long long int) oRememberTimes.GetDiffToLastChunkTime(pDataBlock->GetStreamId(), pDataBlock->GetFileTime()),
                (long long int) oRememberTimes.GetDiffToLastStreamTime(pDataBlock->GetStreamId(), pDataBlock->GetTime()));
        }
    }

    fclose(hCSVFile);
    hCSVFile = NULL;

    // close file
    pFileReader->Close();
    IADTFFileReader::Release(pFileReader);
    return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------

int ADTFStreamingLib::QueryFileInfo(const char* strFilename, std::map<std::string,std::string> &header, std::vector<sStreamInfo> &streams)
{
    // open file
    IADTFFileReader::SetLogLevel(eLOG_LVL_ERROR);
    IADTFFileReader* pFileReader = IADTFFileReader::Create();
    int result;

    if (IS_FAILED(result = pFileReader->Open(strFilename))) {
        IADTFFileReader::Release(pFileReader);
        return(result);
    }

    cout << "File: " << strFilename << endl;

    const tADTFMediaDescriptor* pDescriptor = pFileReader->GetMediaDescriptor();

    int nStreamCount = 0;
    pFileReader->GetStreamCount(nStreamCount);

    cout << endl << "File Header" << endl;
    cout << "------------------------------------------------------------------------------" << endl;
    cout << "Date              : " << pDescriptor->strDateTime << endl;
    cout << "Duration          : " << pFileReader->GetTimeDuration() << endl;
    cout << "Short description : " << pDescriptor->strShortDesc << endl;
    cout << "Long description  : " << pDescriptor->strLongDesc << endl;
    cout << "Chunk count       : " << pFileReader->GetDataBlockCount() << endl;
    cout << "Extension count   : " << pFileReader->GetExtensionCount() << endl;
    cout << "Stream count      : " << nStreamCount << endl;

    string strTimeDuration		= std::to_string(pFileReader->GetTimeDuration());
    string strStreamCount		= std::to_string((long long)nStreamCount);
    string strChunkCount		= std::to_string(pFileReader->GetDataBlockCount());
    string strExtensionCount	= std::to_string((long long)pFileReader->GetExtensionCount());

    header.insert(map<string, string>::value_type("Date", pDescriptor->strDateTime));
    header.insert(map<string, string>::value_type("Duration", strTimeDuration));
    header.insert(map<string, string>::value_type("Short description", pDescriptor->strShortDesc));
    header.insert(map<string, string>::value_type("Long description", pDescriptor->strLongDesc));
    header.insert(map<string, string>::value_type("Chunk count", strChunkCount));
    header.insert(map<string, string>::value_type("Extension count", strExtensionCount));
    header.insert(map<string, string>::value_type("Stream count", strStreamCount));

    cout << endl << "Streams" << endl;
    cout << "------------------------------------------------------------------------------" << endl;

    for (int nIdx = 1;  nIdx <= ADTF_STREAMING_MAX_INDEXED_STREAMS;  ++nIdx) {
        const tADTFStreamDescriptor* pStreamDescriptor = pFileReader->GetStreamDescriptor(nIdx);
        if (pStreamDescriptor != NULL) {
            std::string strType;
			int nimgWidth = 0;
			int nimgHeight = 0;
			int nimgBitPerPixel = 0;
		
            switch (pStreamDescriptor->nType) {
            case tADTFStreamDescriptor::SD_AUDIO:
                strType = "Audio";
                break;
            case tADTFStreamDescriptor::SD_CAN:
                strType = "CAN";
                break;
            case tADTFStreamDescriptor::SD_COMMAND:
                strType = "Command";
                break;
            case tADTFStreamDescriptor::SD_INFO:
                strType = "Info";
                break;
            case tADTFStreamDescriptor::SD_FLEXRAY:
                strType = "Flexray";
                break;
            case tADTFStreamDescriptor::SD_NETWORK_DATA:
                strType = "Network data";
                break;
            case tADTFStreamDescriptor::SD_OTHER:
                strType = "Other";
                break;
            case tADTFStreamDescriptor::SD_REFERENCE:
                strType = "Reference";
                break;
            case tADTFStreamDescriptor::SD_STRUCTURED_DATA:
                strType = "Structured Data";
                break;
            case tADTFStreamDescriptor::SD_USER:
                strType = "User";
                break;
            case tADTFStreamDescriptor::SD_VIDEO:
                strType = "Video";
                nimgWidth = pStreamDescriptor->uData.sVideoFormat.sBitmapFormat.nWidth;
                nimgHeight = pStreamDescriptor->uData.sVideoFormat.sBitmapFormat.nHeight;
                nimgBitPerPixel = pStreamDescriptor->uData.sVideoFormat.sBitmapFormat.nBitsPerPixel;
                break;
            case tADTFStreamDescriptor::SD_COMPRESSED_VIDEO: {
                std::ostringstream oStream;
                oStream << "Compressed Video (";
                if (pStreamDescriptor->uData.sCompressedVideoFormat.bValid) {
                    oStream << "Version: " << pStreamDescriptor->uData.sCompressedVideoFormat.nVersion
                            << ", Codec: " << pStreamDescriptor->uData.sCompressedVideoFormat.strCodec;
                } else {
                    oStream << "no codec info avaliable";
                }
                oStream << ")";
                strType = oStream.str();
                break;
            }
            case tADTFStreamDescriptor::SD_NOT_INIT:
                strType = "Not Init";
                break;
            default:
                strType = "Invalid";
                break;
            }

            tUInt64  nChunkSize = 0;
            int nBlockCount = pFileReader->GetDataBlockCount(nIdx);
            cADTFDataBlock *pDataBlock = NULL;
            tTimeStamp stime = 0;

            tInt64 chunkIndex = pFileReader->Seek(nIdx, 0);
            if(chunkIndex >= 0) {
                tResult nRet = pFileReader->Read(&pDataBlock);
                if (IS_OK(nRet)) {
                    const void* pData;
                    nChunkSize = pDataBlock->GetData(&pData);
                }
            }

            cout << "Stream #" << nIdx << " : " << pFileReader->GetStreamName(nIdx) << endl;
            cout << "    Type           : " << strType << endl;
            cout << "    Major-/Subtype : " << pStreamDescriptor->nMajorType << "/" << pStreamDescriptor->nSubType << endl;
            cout << "    Sampletype     : " << pStreamDescriptor->strSampleType << endl;
            cout << "    Mediatype      : " << pStreamDescriptor->strMediaType << endl << endl;
            cout << "    BlockCount     : " << pFileReader->GetDataBlockCount(nIdx) << endl;

            sStreamInfo info;
            info.nStreamId = nIdx;
            info.name = pFileReader->GetStreamName(nIdx);
            info.type = strType;
            info.nType = pStreamDescriptor->nType;
            info.nMajorType = pStreamDescriptor->nMajorType;
            info.nSubType   = pStreamDescriptor->nSubType;
            info.nBlockCount = nBlockCount;
            info.nChunkSize = nChunkSize;

			info.nimgWidth = nimgWidth;
			info.nimgHeight = nimgHeight;
			info.nimgBitPerPixel = nimgBitPerPixel;
            //info.startTime = stime;
            info.lduration = pFileReader->GetTimeDuration();
			
            streams.push_back(info);
        }
    }

    // close file
    pFileReader->Close();
    IADTFFileReader::Release(pFileReader);

    return 0;
}

int ADTFStreamingLib::CopyChunks(const char* strFilename, const char* targetFilename, std::vector<sStreamInfo> streamInfo, float percentage)
{
    if (strFilename == NULL || targetFilename == NULL || percentage == 0.0) {
        printf("usage: makeDat.exe source-DAT-File destination-DAT-File {save-percentage:default 100}\n");
        return -1;
        
    }
    //initialize the Copier Instance
    IChunkCopy * pCopier = IChunkCopy::Create();

    //open the file to copy stream parts from
    if (IS_FAILED(pCopier->Open(strFilename))){
        printf("FAILED to open SOURCE FILE\n");
        IChunkCopy::Release(pCopier);
        return 2;
    }

    //create the new where to copy the streams to
    if (IS_FAILED(pCopier->CreateFile(targetFilename))){
        printf("FAILED to create DESTINATION FILE\n");
        IChunkCopy::Release(pCopier);
        return 2;
    }
    int targetPacketCount = 0;
    int totalPacketCount = 0;
    for (vector<sStreamInfo>::iterator it = streamInfo.begin(); it != streamInfo.end(); it++){
        
        const tChar* strStreamName = it->name.c_str();
        tUInt16 ui16StreamId = 0;
        if (IS_FAILED(pCopier->GetStreamId(strStreamName, ui16StreamId))) {
            printf("FAILED to resolve STREAM ID\n");
            IChunkCopy::Release(pCopier);
            return 3;
        }
        totalPacketCount += it->nBlockCount;

        // copy the Stream Information to the new file with a new stream id and a different name
        // you can also add more than one stream at once!
        if (IS_FAILED(pCopier->AddStreamToCopy(ui16StreamId,
            ui16StreamId,
            strStreamName))) {
            printf("FAILED to create STREAM\n");
            IChunkCopy::Release(pCopier);
            return 4;
        }
    }
    if (totalPacketCount == 0) {
        printf("FAILED to check totalPacketCount = 0\n");
        return 5;
    }
    targetPacketCount = (int)(((float)totalPacketCount / 100.0) * percentage);
    printf("totalPacketCount=%d save %d%% \nfinal-save-PacketCount=%d \n", totalPacketCount, percentage, targetPacketCount);

    //copy all data blocks of the streams you added with AddStreamToCopy
    // you can also seek to a special position and copy from there
    while (IS_OK(pCopier->CopyNext()) && targetPacketCount--){
       printf("current block position: %i - targetPacketCount=%d\n", static_cast<tInt>(pCopier->GetCurrentBlockPos()), targetPacketCount);
    }

    //clean up
    if (IS_FAILED(pCopier->Close())){
        printf("FAILED to close files properly\n");
        IChunkCopy::Release(pCopier);
        return 5;
    }
    // destroy the copier instance
    IChunkCopy::Release(pCopier);
    return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------