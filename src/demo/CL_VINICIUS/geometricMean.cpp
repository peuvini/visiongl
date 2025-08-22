/*****************************************************************************
 * demo_noise_plus_gmean.cpp – ruído + média geométrica (OpenCL)
 * Uso: ./demo_noise_plus_gmean <lena_gray> <noise.pgm> <win>
 *      grava arquivos em /tmp
 *****************************************************************************/
#ifdef __OPENCL__

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h"   // vglClGeometricMean

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

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("Uso: %s <lena_gray.tif> <noise.pgm> <win>\n", argv[0]);
        return 1;
    }
    const char* lenaPath  = argv[1];
    const char* noisePath = argv[2];
    int win = std::atoi(argv[3]);
    if (win % 2 == 0 || win < 1) {
        fprintf(stderr, "win deve ser ímpar > 0\n");
        return 1;
    }

    /* 1. Carrega 8U cinza */
    IplImage* lena8  = cvLoadImage(lenaPath , CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* noise8 = cvLoadImage(noisePath, CV_LOAD_IMAGE_GRAYSCALE);
    if (!lena8 || !noise8) { fprintf(stderr,"Falha ao carregar.\n"); return 1; }

    cvSaveImage("/tmp/lena_gray_in.tif", lena8);
    cvSaveImage("/tmp/noise_in.pgm",     noise8);

    /* 2. Soma em float (centraliza ruído) */
    CvSize sz = cvGetSize(lena8);
    IplImage* lenaF  = cvCreateImage(sz, IPL_DEPTH_32F, 1);
    IplImage* noiseF = cvCreateImage(sz, IPL_DEPTH_32F, 1);
    cvConvertScale(lena8 , lenaF , 1.0, 0.0);
    cvConvertScale(noise8, noiseF, 1.0, -128.0);

    IplImage* sumF = cvCreateImage(sz, IPL_DEPTH_32F, 1);
    cvAdd(lenaF, noiseF, sumF, nullptr);

    IplImage* sum8 = cvCreateImage(sz, IPL_DEPTH_8U, 1);
    cvConvertScale(sumF, sum8, 1.0, 1.0);          // evita zeros
    cvSaveImage("/tmp/lena_ruido_gray.tif", sum8);

    cvReleaseImage(&lenaF); cvReleaseImage(&noiseF); cvReleaseImage(&sumF);

    /* 3. OpenCL – média geométrica */
    vglClInit();

    char tmpN[]="/tmp/vgl_tmp_gmXXXX.tif"; int fd=mkstemps(tmpN,4); close(fd);
    cvSaveImage(tmpN, sum8);

    VglImage* vIn  = vglLoadImage(tmpN, CV_LOAD_IMAGE_UNCHANGED, 0);
    VglImage* vOut = vglCreateImage(vIn);
    vglClUpload(vIn); vglClUpload(vOut);

    vglClGeometricMean(vIn, vOut, win);  
    vglClFlush();

    vglSetContext(vOut, VGL_CL_CONTEXT);
    vglClDownload(vOut);
    cvSaveImage("/tmp/result_gmean.tif", vOut->ipl);

    /* 4. Limpeza */
    vglReleaseImage(&vIn); vglReleaseImage(&vOut);
    cvReleaseImage(&lena8); cvReleaseImage(&noise8); cvReleaseImage(&sum8);
    unlink(tmpN); vglClFlush();

    printf("Arquivos gerados em /tmp:\n"
           "  lena_gray_in.tif, noise_in.pgm, lena_ruido_gray.tif, result_gmean.tif\n");
    return 0;
}
#endif /* __OPENCL__ */
