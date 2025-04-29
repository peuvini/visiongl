// demo_harmonicMean.cpp
#ifdef __OPENCL__
// Este programa requer OpenCL + VisionGL

#include <cstdio>
#include <cstdlib>
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h"        // wrapper da harmonicMean
#include "demo/timer.h"             // TimerStart, TimerElapsed

int main(int argc, char* argv[])
{
    if(argc != 4) {
        printf("Uso: %s <img_base> <n_iter> <out_path>\n", argv[0]);
        printf("Exemplo: %s lena_gray.tif 100 /tmp\n", argv[0]);
        return 1;
    }

    char*    inPath   = argv[1];
    int      nIter    = atoi(argv[2]);
    const char* outDir = argv[3];

    // Inicializa OpenCL/VisionGL
    vglClInit();

    // Carrega imagem (grayscale ou converte se necessário)
    VglImage* img = vglLoadImage(inPath, CV_LOAD_IMAGE_UNCHANGED, 0);
    if(!img) {
        printf("Erro: não foi possível carregar %s\n", inPath);
        return 1;
    }
    if(img->nChannels != 1) {
        // converter para 1 canal (CPU)
        IplImage* gray = cvCreateImage(cvGetSize(img->ipl), IPL_DEPTH_8U, 1);
        cvCvtColor(img->ipl, gray, CV_BGRA2GRAY);
        img->ipl = gray;
        img->nChannels = 1;
    }

    // Cria imagem de saída
    VglImage* out = vglCreateImage(img);

    // Primeira chamada (warm-up)
    TimerStart();
    vglClHarmonicMean(img, out);
    vglClFlush();
    printf("Primeira call vglClHarmonicMean: %s\n", getTimeElapsedInSeconds());

    // Loop de benchmark
    TimerStart();
    for(int i = 0; i < nIter; ++i) {
        vglClHarmonicMean(img, out);
    }
    vglClFlush();
    printf("Tempo para %d iterações: %s\n", nIter, getTimeElapsedInSeconds());

    // Download para RAM e salva
    vglCheckContext(out, VGL_RAM_CONTEXT);
    char outFile[512];
    snprintf(outFile, sizeof(outFile), "%s/out_harmonic.pgm", outDir);
    printf("Salvando resultado em %s\n", outFile);
    cvSaveImage(outFile, out->ipl);

    // Limpeza
    vglReleaseImage(&img);
    vglReleaseImage(&out);

    return 0;
}
#endif // __OPENCL__
