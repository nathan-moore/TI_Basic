#include <memory>
#include <vector>

#include "Driver.hh"

int main(int argc, char** argv)
{
    if (argc < 2)
        return -1;

    driver d;
    d.parse(argv[1]);
    d.Compile();
}