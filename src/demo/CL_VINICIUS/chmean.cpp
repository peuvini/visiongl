/****************************************************************************
 * demo_noise_plus_chmean.cpp – Aplica ruído SAL‑E‑PIMENTA (mapa 0/128/255)
 *                              e filtra com MÉDIA CONTRA‑HARMÔNICA (VisionGL)
 *
 * Fluxo resumido
 *  1. carrega Lena (8‑bits gray) e mapa_sp (0,128,255)
 *  2. gera imagem ruidosa:
 *        n == 128 → mantém pixel da Lena
 *        n ==   0 → pepper   (preto)
 *        n == 255 → salt     (branco)
 *  3. converte p/ RGBA, envia ao OpenCL
 *  4. executa vglClContraHarmonicMean(win, Q)
 *  5. salva estágios em /tmp para depuração.
 *
 *  Uso:
 *     ./demo_noise_plus_chmean  lena_gray.tif  mapa_sp.pgm  <win>  <Q>
 *
 *          lena_gray.tif  → imagem 8U cinza  (0‑255)
 *          mapa_sp.pgm    → ruído   0/128/255 (gerado por salt_pepper_noise_field)
 *          win            → lado da janela (ímpar: 3,5,7…)
 *          Q              → ordem do filtro  (Q>0 remove pepper,  Q<0 remove salt)
 *
 *  Compilação típica:
 *     g++ demo_noise_plus_chmean.cpp -o demo_chmean \
 *         `pkg-config --cflags --libs opencv4` -lOpenCL -lVisionGL
 ****************************************************************************/
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
#include <fcntl.h>

/* Gray 8U → RGBA 8U */
static IplImage* gray8_to_rgba8(IplImage* g8)
{
    IplImage* bgr = cvCreateImage(cvGetSize(g8), IPL_DEPTH_8U, 3);
    IplImage* rgba = cvCreateImage(cvGetSize(g8), IPL_DEPTH_8U, 4);
    cvCvtColor(g8, bgr, CV_GRAY2BGR);
    cvCvtColor(bgr, rgba, CV_BGR2RGBA);
    cvReleaseImage(&bgr);
    return rgba;
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        printf("Uso: %s <lena_gray.tif> <mapa_sp.pgm> <win> <Q>\n", argv[0]);
        return 1;
    }
    const char* lenaPath  = argv[1];
    const char* mapPath   = argv[2];
    int   win   = std::atoi(argv[3]);
    float Q     = std::atof(argv[4]);

    if (win % 2 == 0 || win < 1) {
        fprintf(stderr, "Erro: win deve ser ímpar (3,5,7…).\n");
        return 1;
    }

    /* 1. lê imagens */
    IplImage* lena8  = cvLoadImage(lenaPath , CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* map8  = cvLoadImage(mapPath  , CV_LOAD_IMAGE_GRAYSCALE);
    if (!lena8 || !map8) {
        fprintf(stderr, "Falha ao carregar imagens.\n");
        return 1;
    }
    if (lena8->width != map8->width || lena8->height != map8->height) {
        fprintf(stderr, "Lena e mapa devem ter mesma dimensão.\n");
        return 1;
    }

    cvSaveImage("/tmp/lena_in.tif", lena8);
    cvSaveImage("/tmp/map_sp_in.pgm", map8);

    /* 2. aplica ruído */
    CvSize sz = cvGetSize(lena8);
    IplImage* noisy8 = cvCreateImage(sz, IPL_DEPTH_8U, 1);

    for (int y = 0; y < sz.height; ++y) {
        unsigned char* pLen = (unsigned char*)(lena8->imageData + y*lena8->widthStep);
        unsigned char* pMap = (unsigned char*)(map8 ->imageData + y*map8 ->widthStep);
        unsigned char* pOut = (unsigned char*)(noisy8->imageData + y*noisy8->widthStep);
        for (int x = 0; x < sz.width; ++x) {
            unsigned char n = pMap[x];
            pOut[x] = (n == 128) ? pLen[x] : n;   // 0/255 substituem; 128 mantém
        }
    }
    cvSaveImage("/tmp/noisy.tif", noisy8);

    /* 3. duplica canais ➜ RGBA */
    IplImage* rgba8 = gray8_to_rgba8(noisy8);

    /* 4. VisionGL + OpenCL */
    vglClInit();

    char tmpN[] = "/tmp/vgl_tmpXXXXXX.tif";
    int fd = mkstemps(tmpN, 4); close(fd);
    cvSaveImage(tmpN, rgba8);

    VglImage* vInp = vglLoadImage(tmpN, CV_LOAD_IMAGE_UNCHANGED, 0);
    VglImage* vOut = vglCreateImage(vInp);

    vglClUpload(vInp);
    vglClUpload(vOut);

    vglClContraHarmonicMean(vInp, vOut, win, Q);
    vglClFlush();

    vglSetContext(vOut, VGL_CL_CONTEXT);
    vglClDownload(vOut);
    cvSaveImage("/tmp/result_chmean.tif", vOut->ipl);

    /* 5. limpeza */
    vglReleaseImage(&vInp);
    vglReleaseImage(&vOut);
    cvReleaseImage(&lena8);
    cvReleaseImage(&map8);
    cvReleaseImage(&noisy8);
    cvReleaseImage(&rgba8);
    unlink(tmpN);

    printf("Fases salvas em /tmp (lena_in, map_sp_in, noisy, result_chmean).\n");
    return 0;
}
#endif /* __OPENCL__ */
