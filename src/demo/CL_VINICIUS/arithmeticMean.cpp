/******************************************************************************
 * demo_addNoise_and_mean.cpp
 *  - Lê Lena + PGM ruído (0-255)
 *  - Converte para float, centraliza ruído (-127), aplica ganho g
 *  - Salva lenaruido_gray.tif   (1 canal 8-bits)
 *  - Empilha para RGBA, envia p/ OpenCL, filtra média aritmética
 ******************************************************************************/
#ifdef __OPENCL__

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h"

#ifdef __OPENCV__
  #include <opencv2/imgproc/imgproc_c.h>
  #include <opencv2/highgui/highgui_c.h>
#else
  #include <vglOpencv.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

/*--------- util Cinza-8U → Cinza-32F --------------------------------------*/
static IplImage* gray8_to_gray32(IplImage* g8)
{
    IplImage* gF = cvCreateImage(cvGetSize(g8), IPL_DEPTH_32F, 1);
    cvConvertScale(g8, gF, 1.0, 0.0);
    return gF;
}

/*--------- empilha Gray-8U → RGBA-8U --------------------------------------*/
static IplImage* gray8_to_rgba8(IplImage* g8)
{
    IplImage* tmp3 = cvCreateImage(cvGetSize(g8), IPL_DEPTH_8U, 3);
    IplImage* tmp4 = cvCreateImage(cvGetSize(g8), IPL_DEPTH_8U, 4);
    cvCvtColor(g8, tmp3, CV_GRAY2BGR);
    cvCvtColor(tmp3, tmp4, CV_BGR2RGBA);
    cvReleaseImage(&tmp3);
    return tmp4;
}

int main(int argc,char*argv[])
{
    if(argc!=5){
        printf("Uso: %s <lena_gray> <noise_gray> <out_dir> <janela>\n",argv[0]);
        return 1;
    }
    const char* lenaPath=argv[1];
    const char* noisePath=argv[2];
    const char* outDir=argv[3];
    int win=atoi(argv[4]); if(win%2==0||win<1){fprintf(stderr,"janela ímpar!\n");return 1;}

    /*--- 1. carrega 8-bits gray -------------------------------------------*/
    IplImage* lena8=cvLoadImage(lenaPath ,CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* noise8=cvLoadImage(noisePath,CV_LOAD_IMAGE_GRAYSCALE);
    if(!lena8||!noise8){fprintf(stderr,"Falha ao carregar.\n");return 1;}

    /*--- 2. converte p/ float e centraliza ruído ---------------------------*/
    IplImage* lenaF  = gray8_to_gray32(lena8);
    IplImage* noiseF = gray8_to_gray32(noise8);

    const float gain = 20.0f;          // ajuste a intensidade: 10-30 típico
    cvAddS(noiseF, cvScalar(-127.0), noiseF);  // média 0
    cvConvertScale(noiseF, noiseF, gain/127.0, 0.0);

    /*--- 3. soma em float ---------------------------------------------------*/
    IplImage* sumF = cvCreateImage(cvGetSize(lenaF), IPL_DEPTH_32F, 1);
    cvAdd(lenaF, noiseF, sumF);
    cvReleaseImage(&lenaF); cvReleaseImage(&noiseF);

    /*--- 4. clip 0-255 e volta p/ 8-bits -----------------------------------*/
    IplImage* sum8 = cvCreateImage(cvGetSize(sumF), IPL_DEPTH_8U, 1);
    cvConvertScale(sumF, sum8, 1.0, 0.0);
    cvReleaseImage(&sumF);

    char noisyGray[256]; snprintf(noisyGray,sizeof(noisyGray),"%s/lenaruido_gray.tif",outDir);
    cvSaveImage(noisyGray, sum8);
    printf("Salvo %s (cinza com ruído)\n", noisyGray);

    /*=======================================================================*
     *  PARTE OpenCL  (empilha RGBA 8-bits → média aritmética)               *
     *=======================================================================*/
    vglClInit();

    IplImage* sumRGBA = gray8_to_rgba8(sum8);             // agora 4 canais
    cvSaveImage((std::string(outDir)+"/lena_ruido_rgba.tif").c_str(), sumRGBA);

    /* usa arquivo tmp para garantir VglImage correto */
    char tmpN[]="/tmp/vgl_tempXXXXXX.tif"; int fd=mkstemps(tmpN,4); close(fd);
    cvSaveImage(tmpN,sumRGBA);

    VglImage* vSum  = vglLoadImage(tmpN,CV_LOAD_IMAGE_UNCHANGED,0);
    VglImage* vMean = vglCreateImage(vSum);
    vglClUpload(vSum); vglClUpload(vMean);

    vglClArithmeticMean(vSum,vMean,win);
    vglClFlush();

    vglSetContext(vMean,VGL_CL_CONTEXT);
    vglClDownload(vMean);

    char outMean[256]; snprintf(outMean,sizeof(outMean),"%s/result_mean.tif",outDir);
    cvSaveImage(outMean, vMean->ipl);
    printf("Salvo %s (suavizado)\n", outMean);

    /* limpeza */
    vglReleaseImage(&vSum); vglReleaseImage(&vMean);
    cvReleaseImage(&lena8); cvReleaseImage(&noise8);
    cvReleaseImage(&sum8);  cvReleaseImage(&sumRGBA);
    unlink(tmpN);
    vglClFlush();
    return 0;
}
#endif /* __OPENCL__ */
