#include <mp2tp/AccessUnit.h>
#include <mp2tp/tspacker.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

#ifdef WIN32
#define fprintf_s fprintf
#else
#define _MAX_PATH 260
#endif

void createFilename(const char* ifname, const char* ext, char* ofname);

int main(int argc, char* argv[])
{
    int ret = 0;

    std::vector<unsigned char> vec(176, 0x2a);

    lcss::AccessUnit au(vec.data(), vec.size(), 0xBD, 0, 0);

    lcss::TSPacker packer;

    size_t cc = 0;
    auto ts = packer.packetizePES(au, 255, &cc, 0);

    return ret;
}

void createFilename(const char* ifname, const char* newext, char* ofname)
{
#ifdef WIN32
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    _splitpath_s(ifname, drive, dir, fname, ext);

    _makepath_s(ofname, _MAX_PATH, drive, dir, fname, newext);
#else
    sprintf(ofname, "%s.%s", ifname, newext);
#endif
}
