/*****************************************************************************
 * demo_noise_plus_hmean.cpp  –  Lena + ruído + filtro de MÉDIA HARMÔNICA
 *
 * Salva todas as fases em /tmp para fácil inspeção.
 * Uso: ./demo_noise_plus_hmean <lena_gray.tif> <noise.pgm> <qualquer_dir> <win>
 *            <lena_gray.tif>  : imagem 8‑bits em escala de cinza (0‑255)
 *            <noise.pgm>      : ruído (PGM, 8‑bits) centrado em 128
 *            <qualquer_dir>   : argumento ignorado (mantido p/ compat.)
 *            <win>            : tamanho da janela (ímpar: 3,5,7…)
 *
 * A média harmônica é calculada em OpenCL pelo kernel `vglClHarmonicMean.cl`.
 *
 * Requer: VisionGL + OpenCL + OpenCV  (ou vglOpencv.h se OpenCV não estiver
 * disponível na máquina‑alvo).
 *****************************************************************************/
#ifdef __OPENCL__

#include "vglClImage.h"
#include "vglContext.h"
#include "cl2cpp_vinicius.h"      // vglClHarmonicMean

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
#include <fcntl.h>                 // mkstemps

/*------------------------------------------------------------------*
 *  Gray 8U  ➜  RGBA 8U (canal duplicado)
 *------------------------------------------------------------------*/
static IplImage* gray8_to_rgba8(IplImage* g8)
{
    IplImage* tmp3 = cvCreateImage(cvGetSize(g8), IPL_DEPTH_8U, 3);
    IplImage* tmp4 = cvCreateImage(cvGetSize(g8), IPL_DEPTH_8U, 4);
    cvCvtColor(g8, tmp3, CV_GRAY2BGR);
    cvCvtColor(tmp3, tmp4, CV_BGR2RGBA);
    cvReleaseImage(&tmp3);
    return tmp4;
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        printf("Uso: %s <lena_gray.tif> <noise.pgm> <_ignorado_> <win>\n", argv[0]);
        return 1;
    }
    const char* lenaPath  = argv[1];
    const char* noisePath = argv[2];
    int         win       = std::atoi(argv[4]);

    if (win % 2 == 0 || win < 1) {
        fprintf(stderr,"Janela deve ser ímpar (3,5,7…).\n");
        return 1;
    }

    /*------------------------------------------------------------------*
     * 1. Carrega Lena e ruído                                           *
     *------------------------------------------------------------------*/
    IplImage* lena8  = cvLoadImage(lenaPath , CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* noise8 = cvLoadImage(noisePath, CV_LOAD_IMAGE_GRAYSCALE);
    if (!lena8 || !noise8) {
        fprintf(stderr,"Falha ao carregar as imagens de entrada.\n");
        return 1;
    }

    cvSaveImage("/tmp/gray_in_img.tif", lena8);
    cvSaveImage("/tmp/noise_in_img.pgm",     noise8);

    /*------------------------------------------------------------------*
     * 2. Soma em float (32F), sem saturar                               *
     *------------------------------------------------------------------*/
    CvSize sz = cvGetSize(lena8);

    IplImage* lenaF  = cvCreateImage(sz, IPL_DEPTH_32F, 1);
    IplImage* noiseF = cvCreateImage(sz, IPL_DEPTH_32F, 1);

    cvConvertScale(lena8 , lenaF , 1.0, 0.0);     // 0‑255 ➜ mesmo valor (float)
    cvConvertScale(noise8, noiseF, 1.0, -128.0);  // ruído centrado em 0

    IplImage* sumF   = cvCreateImage(sz, IPL_DEPTH_32F, 1);
    cvAdd(lenaF, noiseF, sumF, nullptr);

    IplImage* sum8   = cvCreateImage(sz, IPL_DEPTH_8U, 1);
    cvConvertScale(sumF, sum8, 1.0, 1.0);         // +1 ➜ evita zeros absolutos
    cvSaveImage("/tmp/ruido_gray.tif", sum8);

    /* libera temporários float */
    cvReleaseImage(&lenaF);
    cvReleaseImage(&noiseF);
    cvReleaseImage(&sumF);

    /*------------------------------------------------------------------*
     * 3. Duplica canais ➜ RGBA (VisionGL prefere 4 canais)             *
     *------------------------------------------------------------------*/
    IplImage* rgba8 = gray8_to_rgba8(sum8);

    /*------------------------------------------------------------------*
     * 4. OpenCL – filtro de média harmônica                             *
     *------------------------------------------------------------------*/
    vglClInit();

    char tmpN[] = "/tmp/vgl_tmpXXXXXX.tif"; // arquivo temporário para carregar em VGL
    int fd = mkstemps(tmpN, 4); close(fd);
    cvSaveImage(tmpN, rgba8);

    VglImage* vInp  = vglLoadImage(tmpN, CV_LOAD_IMAGE_UNCHANGED, 0); // RGBA 8U
    VglImage* vOut  = vglCreateImage(vInp);

    vglClUpload(vInp);
    vglClUpload(vOut);

    vglClHarmonicMean(vInp, vOut, win);
    vglClFlush();

    /* devolve ao host */
    vglSetContext(vOut, VGL_CL_CONTEXT);
    vglClDownload(vOut);
    cvSaveImage("/tmp/result_HarmonicMean.tif", vOut->ipl);

    /*------------------------------------------------------------------*
     * 5. Limpeza                                                        *
     *------------------------------------------------------------------*/
    vglReleaseImage(&vInp);
    vglReleaseImage(&vOut);
    cvReleaseImage(&lena8);
    cvReleaseImage(&noise8);
    cvReleaseImage(&sum8);
    cvReleaseImage(&rgba8);
    unlink(tmpN);

    vglClFlush();

    printf("Todas as imagens foram gravadas em /tmp\n");
    return 0;
}
#endif /* __OPENCL__ */