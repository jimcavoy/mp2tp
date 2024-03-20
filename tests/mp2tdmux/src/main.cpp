// mp2tdmux.cpp : Defines the entry point for the console application.
// MPEG-2 TS Demultiplexor - an application to demultiplex a MPEG-2 TS file
// and output the elementary streams for each media type into a file

#include "MP2TFileDemux.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

const int N = 188*7;

void createFilename(const char* ifname, const char* ext, char* ofname);

int main(int argc, char* argv[])
{
	int ret = 0;
	ifstream ifile;
	ofstream ofileh264;
	ofstream ofileKlv;
	ofstream ofileAudio;
	ofstream ofileExi;

	do
	{
		if(argc != 2)
		{
			cerr << "usage: mp2tdmux tsfile" << endl;
			ret = 1;
			break;
		}

		ifile.open(argv[1],std::ios::in|std::ios::binary);
		if(!ifile)
		{
			cerr << "Fail to open file " << argv[1];
			ret = 2;
			break;
		}

		char ofnameh264[_MAX_PATH];
		char ofnameKlv[_MAX_PATH];
		char ofnameAudio[_MAX_PATH];
		char ofnameExi[_MAX_PATH];
		createFilename(argv[1],"h264",ofnameh264);
		createFilename(argv[1],"klv",ofnameKlv);
		createFilename(argv[1],"mp2",ofnameAudio);
		createFilename(argv[1],"exi", ofnameExi);

		ofileh264.open(ofnameh264,std::ios::out|std::ios::binary);
		if(!ofileh264)
		{
			cerr << "Fail to open file " << ofnameh264;
			ret = 3;
			break;
		}

		ofileKlv.open(ofnameKlv,std::ios::out|std::ios::binary);
		if(!ofileKlv)
		{
			cerr << "Fail to open file " << ofnameKlv;
			ret = 4;
			break;
		}

		ofileAudio.open(ofnameAudio,std::ios::out|std::ios::binary);
		if(!ofileAudio)
		{
			cerr << "Fail to open file " << ofnameAudio;
			ret = 5;
			break;
		}

		ofileExi.open(ofnameExi, std::ios::out | std::ios::binary);
		if (!ofileExi)
		{
			cerr << "Fail to open file " << ofnameExi;
			ret = 6;
			break;
		}

		BYTE memblock[N];
		MP2TFileDemux demux(ofileh264,ofileKlv,ofileAudio,ofileExi);
		try
		{
			clock_t start, end;
			start = clock();

			while(ifile.good())
			{
				ifile.read((char*)memblock,N);
				demux.parse(memblock,(unsigned)ifile.gcount());
			}

			end = clock();
			double dif = (double)(end - start) / CLOCKS_PER_SEC;
			fprintf_s(stdout, "Elapsed time is %2.3lf seconds.\n", dif);
		}
		catch(...)
		{
			cerr << "Unknown exception thrown\n";
			ret = -1;
			break;
		}
	}
	while(false);

	ifile.close();
	ofileh264.close();
	ofileKlv.close();
	ofileAudio.close();
	return ret;
}

void createFilename(const char* ifname, const char* newext, char* ofname)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s(ifname,drive,dir,fname,ext);

	_makepath_s(ofname,_MAX_PATH,drive,dir,fname,newext);
}
