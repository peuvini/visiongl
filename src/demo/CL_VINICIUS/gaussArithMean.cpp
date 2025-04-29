#ifdef __OPENCL__
#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h"
#include "glsl2cpp_shaders.h"
#include "demo/timer.h"

#ifdef __OPENCV__
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

/* ------------ protótipos ---------------------------------------- */
static void cpuRandomNoise(VglImage* dst);
static void saveToTmp(VglImage* img, const char* fname);

/*------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("\nUso: %s <img.tif> <tam_média>\n"
               "Ex.: %s images/lena_gray.tif 3\n\n",
               argv[0], argv[0]);
        return 1;
    }

    const char* imgName = argv[1];
    int   wsize         = atoi(argv[2]);          // janela (ímpar)

    if (wsize % 2 == 0) { puts("tam_média deve ser ímpar"); return 1; }

    vglClInit();

    /* 1. Carrega Lena grayscale ----------------------------------- */
    VglImage* in = vglLoadImage((char*)imgName, CV_LOAD_IMAGE_GRAYSCALE, 0);
    if (!in) { fprintf(stderr,"Erro ao abrir %s\n", imgName); return 1; }

    /* 2. Cria imagens --------------------------------------------- */
    VglImage* noise    = vglCreateImage(in);   // ruído CPU
    VglImage* noisy    = vglCreateImage(in);   // soma
    VglImage* filtered = vglCreateImage(in);   // resultado final

    /* 3. Gera ruído na CPU ---------------------------------------- */
    cpuRandomNoise(noise);                     // preenche em RAM
    vglSetContext(noise, VGL_RAM_CONTEXT);
    vglClUpload(noise);                        // envia GPU (necessário p/ vglSum)

    /* 4. Soma + Média na GPU -------------------------------------- */
    TimerStart();
    vglSum(in, noise, noisy);                   // soma
    vglClArithmeticMean(noisy, filtered, wsize);// média aritmética
    vglClFlush();
    printf("Pipeline concluído em %s\n", getTimeElapsedInSeconds());

    /* 5. Salva em /tmp -------------------------------------------- */
    saveToTmp(noise,    "/tmp/noise_cpu.tif");
    saveToTmp(noisy,    "/tmp/noisy.tif");
    saveToTmp(filtered, "/tmp/filtered_mean.tif");

    /* 6. Libera ---------------------------------------------------- */
    vglReleaseImage(&in);
    vglReleaseImage(&noise);
    vglReleaseImage(&noisy);
    vglReleaseImage(&filtered);
    return 0;
}
/*----------- gera ruído uniforme [0,1] em float -------------------*/
static void cpuRandomNoise(VglImage* dst)
{
    CvRNG rng = cvRNG((uint64) time(nullptr));
    CvMat mat;
    cvGetMat(dst->ipl, &mat);
    cvRandArr(&rng, &mat, CV_RAND_UNI, cvScalar(0.0f), cvScalar(1.0f));
}
/*------------------------------------------------------------------*/
static void saveToTmp(VglImage* img, const char* fname)
{
    vglSetContext(img, VGL_CL_CONTEXT);
    vglClDownload(img);                   // GPU → RAM
    vglCheckContext(img, VGL_RAM_CONTEXT);
    cvSaveImage((char*)fname, img->ipl);
    printf("Salvo: %s\n", fname);
}
#endif
