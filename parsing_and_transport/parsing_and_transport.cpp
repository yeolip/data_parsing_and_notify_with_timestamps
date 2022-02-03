/**
 *
 * ADTF File parsing and transport example(using ADTF library and TimeStamps)
 *
 * @file
 *
 * $Author: yeolip.yoon $
 * $Date: 2022-01-26   $
 *
 * @remarks
 *
 */

//#define _RUN_TEST_IMAGEBUFFERALLOC_
//#define _RUN_MAKE_USER_DAT_


//#ifdef WIN32
//#include <windows.h>
//#endif

#include <sstream>
#include <adtf_streaming.h>
#include "ImageBufferCtrl.h"

//using namespace adtfstreaming;

#define WORKING_FILE_PATH_SIZE 256
#define MAX_FRAMEBUFFER         10
#ifdef __linux
    #define strcpy_s(dest, size, source) strcpy(dest, source)
#endif

FILE* _fopen(const char* strFileName, const char* mode)
{
#ifdef WIN32
    FILE* pFileHandle = NULL;
    if (0 == fopen_s(&pFileHandle, strFileName, mode))
    {
        return pFileHandle;
    }
#else
    return fopen(strFileName, mode);
#endif
    return NULL;
}


/////////////////////////////////////////////////////////////////
#ifdef _RUN_TEST_IMAGEBUFFERALLOC_
void TEST_ImageBufferAlloc(int slot) {

	//int slot = 2;
	ImageBufferAlloc imgAlloc;
	imgAlloc.createImageBuffer(1280, 720, 1, 5, 3);
	int imgIdx = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx2 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx3 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx4 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx5 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	unsigned char* taddr = NULL;
	int tsize = 0;
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx2, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx2, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx3, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx4, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.acquire_ImageBuffer_slot(slot, imgIdx5, &taddr, &tsize)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx6 = imgAlloc.getAvailIdx_ImageBuffer();
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.release_ImageBuffer_slot(slot, imgIdx)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.release_ImageBuffer_slot(slot, imgIdx2)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.release_ImageBuffer_slot(slot, imgIdx3)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx7 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx8 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx9 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx10 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx11 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	if (imgAlloc.release_ImageBuffer_slot(slot, imgIdx4)) {
		printf("index = %d, taddr= %p\n", imgIdx, taddr);
	}
	else {
		cout << "imgIdx=%d is not acquired buffer." << endl;
	}
	imgAlloc.info_ImageBuffer_slot();
	int imgIdx12 = imgAlloc.getAvailIdx_ImageBuffer_slot(slot);
	imgAlloc.info_ImageBuffer_slot();
	string sname = "hello world\n";
	imgAlloc.copy_to_imageBuffer_slot(slot, imgIdx12, (unsigned char*)sname.c_str(), sname.size());
	//imgAlloc.~ImageBufferAlloc();
	return;
}
#endif

#ifdef _RUN_MAKE_USER_DAT_
void MAKE_userDAT(int argc, char* argv[]) {
	//"C:/Program Files/adtf-streaminglib/1_DRCAM_KOR40BU4578_20190219_114431_0002.dat" "test_het.dat" 10
	//"C:/Program Files/adtf-streaminglib/cms_20211108_105456.dat" "test_cms.dat" 10
	std::map<std::string, std::string> header;
	std::vector<sStreamInfo> streams;
	ImageBufferCtrl imgCtrl;
	
	if (argc <= 2) {
		printf("usage: makeDat.exe source-DAT-File destination-DAT-File {save-percentage:default 100}\n\n");
		return;
	}
	char strWorkingFile[WORKING_FILE_PATH_SIZE];
	imgCtrl.Decompress(argv[1], strWorkingFile);
	imgCtrl.QueryFileInfo(strWorkingFile, header, streams);

	if(argc == 3) imgCtrl.CopyChunks(strWorkingFile, argv[2], streams);
	else 		  imgCtrl.CopyChunks(strWorkingFile, argv[2], streams, atof(argv[3]));

	imgCtrl.AccessFileData(argv[2], "result.csv");
}
#endif


int main(int argc, char* argv[])
{
#ifdef	_RUN_TEST_IMAGEBUFFERALLOC_
	TEST_ImageBufferAlloc(2);
	return 0;
#endif
#ifdef _RUN_MAKE_USER_DAT_
	MAKE_userDAT(argc, argv);
	return 0;
#endif
	//argument
	//"../../../../input/cms_example_short.dat" "../../../../input/het_example_short.dat"
    //"C:/Program Files/adtf-streaminglib/cms_20211108_105456.dat" 
    //"C:/Program Files/adtf-streaminglib/1_DRCAM_KOR40BU4578_20190219_114431_0002.dat" 
    //"C:/Program Files/adtf-streaminglib/cms_20211108_105456.dat" "C:/Program Files/adtf-streaminglib/1_DRCAM_KOR40BU4578_20190219_114431_0002.dat" 

    //if (argc < 3 || argv[1] == NULL || argv[2] == NULL)
    if (argc < 2 || argv[1] == NULL)
    {
        cout << "Please add arguments below, when you running\n usage: " << argv[0] << " source-DAT-File1 {source-DAT-File2}"  << endl;
        return -1;
    }

#if 1
    ImageBufferCtrl mimgCtrl;

    //One DAT parsing and transport
    mimgCtrl.autoPlay(argv[1]);

    //Two DAT parsing and transport
    if (argc == 3)
        mimgCtrl.autoPlay(argv[2]);


#else
    char strWorkingFile[WORKING_FILE_PATH_SIZE];

    //full sequence
    std::map<std::string, std::string> header;
    std::vector<sStreamInfo> streams;
    std::vector<sStreamInfo> vstreams;
    cout << "test code test code test code test code " << endl;

    ImageBufferCtrl mimgCtrl;

    mimgCtrl.Decompress(argv[1], strWorkingFile);
    mimgCtrl.QueryFileInfo(strWorkingFile, header, streams);
    vstreams = mimgCtrl.collectVideoStream_and_Set(streams);
    mimgCtrl.start();

    //mimgCtrl.setFPS(30);
    mimgCtrl.parserRawData(strWorkingFile, vstreams, "raw_img_%F_%T_%D");

	cout<<"wait and stop"<<endl;
	mimgCtrl.wait_and_stop();
#endif
    
    cout << "End" << endl;
    return 0;
}
