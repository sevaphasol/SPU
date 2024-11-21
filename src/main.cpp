#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <SFML/Audio.hpp>

#include "stack.h"
#include "proc.h"

int main(const int argc, const char* argv[])
{
    SpuInfo_t spu_info = {};

    //-------------------------------------------------------------------//

    sf::RenderWindow window(sf::VideoMode(GraphicWindowLength, GraphicWindowWidth), GraphicWindowName);
    sf::SoundBuffer buffer;
    sf::Sound sound;

    //-------------------------------------------------------------------//

    if (SpuInfoCtor(&spu_info, &window, argc, argv) != SPU_SUCCESS)
    {
        SpuInfoDtor(&spu_info);

        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    if (StartProgramm(&spu_info) != SPU_SUCCESS)
    {
        SpuInfoDtor(&spu_info);

        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    if (SpuInfoDtor (&spu_info) != SPU_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}
