/******************************************************************************
 * demo_bilateral_cl.cpp
 *   Usa o kernel OpenCL vglClBilateral já disponível na VisionGL.
 *   Mede tempo da 1ª chamada e do loop com N iterações.
 ******************************************************************************/
#ifdef __OPENCL__

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h"

#ifdef __OPENCV__
  #include <opencv2/imgproc/types_c.h>
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

#include "demo/timer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("\nUso  : %s <imagem> <iteracoes> <dir_saida>\n"
               "Exemplo: %s lena_gray.tif 100 /tmp\n\n",
               argv[0], argv[0]);
        return 1;
    }

    /*--------------- argumentos -------------------------------------------*/
    const char* inFilename = argv[1];
    int  nSteps            = std::atoi(argv[2]);
    const char* outPath    = argv[3];
    char outFile[512];

    /*--------------- inicializa OpenCL ------------------------------------*/
    vglClInit();

    /*--------------- carrega imagem ---------------------------------------*/
    VglImage* img = vglLoadImage((char*)inFilename, CV_LOAD_IMAGE_UNCHANGED, 0);
    if (!img)
    {
        fprintf(stderr, "Erro: arquivo não encontrado: %s\n", inFilename);
        return 1;
    }

    /*---  se for RGB, converte p/ 4 canais (rgba) -------------------------*/
    if (img->nChannels == 3)
    {
        img->ndarray ? vglNdarray3To4Channels(img) : vglIpl3To4Channels(img);
    }

    VglImage* out = vglCreateImage(img);

    /*--------------- primeira chamada -------------------------------------*/
    TimerStart();
    vglClBilateral(img, out);
    vglClFlush();
    printf("Primeira chamada ao Bilateral: %s s\n", getTimeElapsedInSeconds());

    /*--------------- loop de benchmark ------------------------------------*/
    TimerStart();
    for (int i = 0; i < nSteps; ++i)
        vglClBilateral(img, out);
    vglClFlush();
    printf("Tempo em %d iterações: %s s\n", nSteps, getTimeElapsedInSeconds());

    /*--------------- salva resultado --------------------------------------*/
    vglCheckContext(out, VGL_RAM_CONTEXT);
    std::snprintf(outFile, sizeof(outFile), "%s/out_cl_bilateral.tif", outPath);
    cvSaveImage(outFile, out->ipl);
    printf("Imagem salva em %s\n", outFile);

    /*--------------- limpeza ----------------------------------------------*/
    vglReleaseImage(&img);
    vglReleaseImage(&out);
    vglClFlush();
    return 0;
}
#endif /* __OPENCL__ */
