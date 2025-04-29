#ifdef __OPENCL__

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

// Headers VisionGL e OpenCL
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h" 
#include "glsl2cpp_shaders.h"

#ifdef __OPENCV__
  #include <opencv2/imgproc/types_c.h>
  #include <opencv2/imgproc/imgproc_c.h>
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

#include "demo/timer.h"

// Exemplo de uso:
//   demo_geometric_mean <input_image> <output_path>
//   Exemplo: ./demo_geometric_mean lena_gray.tif /tmp
//
//     - Carrega "lena_gray.tif"
//     - Lê "random.pgm" para gerar ruído
//     - Aplica ruído gaussiano (mean=0, sigma=0.1, como exemplo)
//     - Aplica o filtro de média geométrica
//     - Salva o resultado em /tmp
int main(int argc, char* argv[])
{
    // Aqui decidi não pedir w_size, pois o kernel vglClGeometricMean pode já ter
    // internamente uma janela, ou podemos adaptar o código para ler esse parâmetro.
    // Ajuste conforme sua implementação real, se precisar de w_size.
    if (argc != 3)
    {
        std::cout << "\nUsage: " << argv[0] << " <input_image> <output_path>\n";
        std::cout << "Example: " << argv[0] << " lena_gray.tif /tmp\n\n";
        return 1;
    }

    // Inicializa contexto do VisionGL + OpenCL
    vglClInit();

    // Lê parâmetros
    char* inFilename = argv[1];
    char* outPath = argv[2];
    char  outFilename[512];

    // Carrega imagem de entrada
    std::cout << "LOADING IMAGE: " << inFilename << std::endl;
    VglImage* img = vglLoadImage(inFilename, CV_LOAD_IMAGE_UNCHANGED, 0);
    if (!img)
    {
        std::cerr << "Error loading image: " << inFilename << std::endl;
        return 1;
    }

    // Se tiver 3 canais, converte para 4
    if (img->nChannels == 3)
    {
        std::cout << "Converting 3 channels to 4 channels..." << std::endl;
        if (img->ndarray) vglNdarray3To4Channels(img);
        else              vglIpl3To4Channels(img);
    }

    // Cria imagens de saída
    VglImage* outNoisy    = vglCreateImage(img);
    VglImage* outGeometric = vglCreateImage(img);

    // Carrega imagem "random.pgm" (ou gere de outra forma)
    std::cout << "LOADING RANDOM IMAGE (random.pgm)..." << std::endl;
    VglImage* randImg = vglLoadImage("/home/peuvini/Documentos/NovoTeste/visiongl/bin/random.pgm",
        CV_LOAD_IMAGE_UNCHANGED, 0);

    if (!randImg)
    {
        std::cerr << "Error: Could not load random image (random.pgm)." << std::endl;
        return 1;
    }

    // Se tiver 3 canais no randImg, converte
    if (randImg->nChannels == 3)
    {
        if (randImg->ndarray) vglNdarray3To4Channels(randImg);
        else                  vglIpl3To4Channels(randImg);
    }

    // Parâmetros para ruído Gaussiano
    float mean  = 0.0f;
    float sigma = 0.1f;

    // Aplica o ruído
    std::cout << "APPLYING GAUSSIAN NOISE..." << std::endl;
    TimerStart();
    vglClGaussianNoise(img, randImg, outNoisy, mean, sigma);
    vglClFlush();
    std::cout << "Time for Gaussian Noise: " << getTimeElapsedInSeconds() << " s\n" << std::endl;

    // Salva imagem com ruído
    vglCheckContext(outNoisy, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s/out_noise_geo.tif", outPath);
    cvSaveImage(outFilename, outNoisy->ipl);
    std::cout << "Saved noisy image to " << outFilename << std::endl;

    // Aplica filtro de média geométrica
    // Se sua implementação requer w_size, adapte:
    //   vglClGeometricMean(outNoisy, outGeometric, w_size);
    // caso contrário, use a função sem o parâmetro adicional.
    std::cout << "APPLYING GEOMETRIC MEAN..." << std::endl;
    TimerStart();
    vglClGeometricMean(outNoisy, outGeometric);
    vglClFlush();
    std::cout << "Time for Geometric Mean: " << getTimeElapsedInSeconds() << " s\n" << std::endl;

    // Salva resultado
    vglCheckContext(outGeometric, VGL_RAM_CONTEXT);
    sprintf(outFilename, "%s/out_geo_mean.tif", outPath);
    cvSaveImage(outFilename, outGeometric->ipl);
    std::cout << "Saved geometric-mean image to " << outFilename << std::endl;

    // Libera memória
    vglReleaseImage(&randImg);
    vglReleaseImage(&outNoisy);
    vglReleaseImage(&outGeometric);
    vglReleaseImage(&img);

    vglClFlush();
    return 0;
}

#endif // __OPENCL__
