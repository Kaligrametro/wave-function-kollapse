#include <iostream>
#include <string>

#include "Bmp.h"
#include "WFC.h"

int main()
{
    const int sampleNumber = 1;
    const std::string filename = "samples/sample(" + std::to_string(sampleNumber) + ").bmp";
    bmp::Bitmap bmp(filename);

    WFC<bmp::Pixel, 2, 7> wfc(
        bmp.getData(),
        { bmp.width(), bmp.height() }
        , Flag::REFLECT | Flag::ROTATE
    );
    wfc.run({ 200,100 });

    std::cout << "Exit success!\n";
    return 0;
}