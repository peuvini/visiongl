#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    char* usage = (char*) "\n\
    This program reads a file and saves a copy. Usage is as follows:\n\
\n\
    demo_io <input file> <output file> <option>\n\
    option:\n\
    -1 -> Load image as is.\n\
     0 -> Load image as grayscale.\n\
     1 -> Load image as BGR.\n\
\n";

    if (argc < 4)
    {
        printf("%s", usage);
        exit(1);
    }

    char *inFilename = argv[1];
    char *outFilename = argv[2];
    int option = atoi(argv[3]);

    Mat img;  // Agora Mat é reconhecido

    switch(option)
    {
        case -1:
            img = imread(inFilename, IMREAD_UNCHANGED);
            break;
        case 0:
            img = imread(inFilename, IMREAD_GRAYSCALE);
            break;
        case 1:
            img = imread(inFilename, IMREAD_COLOR);
            break;
        default:
            img = imread(inFilename);
            break;
    }

    if (img.empty())
    {
        printf("Erro ao carregar a imagem: %s\n", inFilename);
        return -1;
    }

    bool success = imwrite(outFilename, img);
    if (!success)
    {
        printf("Erro ao salvar a imagem: %s\n", outFilename);
        return -1;
    }

    return 0;
}
