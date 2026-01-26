#include <mp2tp/AccessUnit.h>
#include <mp2tp/tspacker.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    int ret = 0;

    if (argc != 2)
    {
        cerr << "Usage: packtest <size>" << endl;
    }

    int size = std::atoi(argv[1]);

    std::vector<unsigned char> vec(size, 0x2a);

    lcss::AccessUnit au(vec.data(), vec.size(), 0xBD, 0, 0);

    lcss::TSPacker packer;

    size_t cc = 0;
    auto ts = packer.packetizePES(au, 255, &cc, 0);

    cout << ts.size() << endl;

    return ret;
}

