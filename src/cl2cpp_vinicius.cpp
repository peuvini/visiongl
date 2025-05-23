
/*********************************************************************
***                                                                 ***
***  Source code generated by cl2cpp.pl                             ***
***                                                                 ***
***  Please do not edit                                             ***
***                                                                 ***
*********************************************************************/

#include "vglImage.h"
#include "vglClImage.h"
#include "vglContext.h"

#include "vglShape.h"
#include "vglClShape.h"

#include "vglStrEl.h"
#include "vglClStrEl.h"

#include <fstream>

extern VglClContext cl;

/** Arithmetic-mean filter.
 *
 *  Para cada pixel (x,y) calcula  
 *      f̂(x,y) = 1/(m·n) · Σ g(s,t) , (s,t) ∈ janela m×n
 *
 *  - `win`  = tamanho da janela (m = n = win) - deve ser ímpar  
 *  - Usa endereço `CLK_ADDRESS_CLAMP` → replica o valor da borda  
 *  - Processa imagens RGBA (UNORM_INT8 ou FLOAT); acumula em `float4`
 */
void vglClArithmeticMean(VglImage* img_input, VglImage* img_output, int win)
{
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_VINICIUS/vglClArithmeticMean.cl";
    printf("Compiling %s\n", _file_path);
    std::ifstream _file(_file_path);
    if(_file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, _file_path);
      exit(1);
    }
    std::string _prog( std::istreambuf_iterator<char>( _file ), ( std::istreambuf_iterator<char>() ) );
    const char *_source_str = _prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", _source_str);
#endif
    _program = clCreateProgramWithSource(cl.context, 1, (const char **) &_source_str, 0, &_err );
    vglClCheckError(_err, (char*) "clCreateProgramWithSource" );
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_VINICIUS/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClArithmeticMean", &_err );
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( int ), &win );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = img_input->getWidthIn();
  if (img_input->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_input->getWidthStepWords();
  }
  if (img_output->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_output->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** 
 * Filtro Bilateral: aplica ponderação espacial e de similaridade (range) para suavização preservando bordas.
 * A função lê a imagem de entrada e escreve a imagem filtrada na saída.
 * Os parâmetros sigma_spatial, sigma_range, window_size_x e window_size_y foram fixados.
 */
void vglClBilateral(VglImage* img_input, VglImage* img_output)
{
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_VINICIUS/vglClBilateral.cl";
    printf("Compiling %s\n", _file_path);
    std::ifstream _file(_file_path);
    if(_file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, _file_path);
      exit(1);
    }
    std::string _prog( std::istreambuf_iterator<char>( _file ), ( std::istreambuf_iterator<char>() ) );
    const char *_source_str = _prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", _source_str);
#endif
    _program = clCreateProgramWithSource(cl.context, 1, (const char **) &_source_str, 0, &_err );
    vglClCheckError(_err, (char*) "clCreateProgramWithSource" );
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_VINICIUS/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClBilateral", &_err );
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = img_input->getWidthIn();
  if (img_input->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_input->getWidthStepWords();
  }
  if (img_output->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_output->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** vglClGaussianNoise
 *
 *  Adiciona ruído gaussiano N(μ, σ²) pixel-a-pixel:
 *      out(x,y) = clamp( in(x,y) + μ + σ·Z , 0 , 1 )
 *  onde Z é obtido pelo método de Box–Muller a partir de dois
 *  números U ~ U(0,1) gerados por um LCG simples (Park-Miller).
 *
 *  Parâmetros
 *      img_input   – imagem de entrada  (UNORM_INT8 ou FLOAT [0-1])
 *      img_output  – imagem de saída    (mesmo formato)
 *      seed        – semente global (mude em cada execução se quiser
 *                    ruídos diferentes)
 *      mean        – μ   (ex.: 0.0f)
 *      sigma       – σ   (ex.: 0.1f → variância 0.01)
 *
 *  O kernel produz **um** número gaussiano por pixel e o aplica a
 *  todos os canais (RGBA).  Para canais independentes basta gerar
 *  quatro amostras em vez de uma.
 */
void vglClGaussianNoise(VglImage* img_input, VglImage* img_output, uint seed, float mean, float sigma)
{
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_VINICIUS/vglClGaussianNoise.cl";
    printf("Compiling %s\n", _file_path);
    std::ifstream _file(_file_path);
    if(_file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, _file_path);
      exit(1);
    }
    std::string _prog( std::istreambuf_iterator<char>( _file ), ( std::istreambuf_iterator<char>() ) );
    const char *_source_str = _prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", _source_str);
#endif
    _program = clCreateProgramWithSource(cl.context, 1, (const char **) &_source_str, 0, &_err );
    vglClCheckError(_err, (char*) "clCreateProgramWithSource" );
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_VINICIUS/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClGaussianNoise", &_err );
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( uint ), &seed );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  _err = clSetKernelArg( _kernel, 3, sizeof( float ), &mean );
  vglClCheckError( _err, (char*) "clSetKernelArg 3" );

  _err = clSetKernelArg( _kernel, 4, sizeof( float ), &sigma );
  vglClCheckError( _err, (char*) "clSetKernelArg 4" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = img_input->getWidthIn();
  if (img_input->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_input->getWidthStepWords();
  }
  if (img_output->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_output->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Geometric mean of src image. Result is stored in dst image.
*/
void vglClGeometricMean(VglImage* img_input, VglImage* img_output)
{
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_VINICIUS/vglClGeometricMean.cl";
    printf("Compiling %s\n", _file_path);
    std::ifstream _file(_file_path);
    if(_file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, _file_path);
      exit(1);
    }
    std::string _prog( std::istreambuf_iterator<char>( _file ), ( std::istreambuf_iterator<char>() ) );
    const char *_source_str = _prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", _source_str);
#endif
    _program = clCreateProgramWithSource(cl.context, 1, (const char **) &_source_str, 0, &_err );
    vglClCheckError(_err, (char*) "clCreateProgramWithSource" );
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_VINICIUS/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClGeometricMean", &_err );
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = img_input->getWidthIn();
  if (img_input->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_input->getWidthStepWords();
  }
  if (img_output->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_output->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Harmonic mean of src image. Result is stored in dst image.
 *
 *  f̂(x,y) = (m·n) / Σ [ 1 / g(s,t) ]
 *  where m = window_size_x, n = window_size_y
 */
void vglClHarmonicMean(VglImage* img_input, VglImage* img_output)
{
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_VINICIUS/vglClHarmonicMean.cl";
    printf("Compiling %s\n", _file_path);
    std::ifstream _file(_file_path);
    if(_file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, _file_path);
      exit(1);
    }
    std::string _prog( std::istreambuf_iterator<char>( _file ), ( std::istreambuf_iterator<char>() ) );
    const char *_source_str = _prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", _source_str);
#endif
    _program = clCreateProgramWithSource(cl.context, 1, (const char **) &_source_str, 0, &_err );
    vglClCheckError(_err, (char*) "clCreateProgramWithSource" );
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_VINICIUS/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClHarmonicMean", &_err );
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = img_input->getWidthIn();
  if (img_input->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_input->getWidthStepWords();
  }
  if (img_output->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = img_output->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** 
 * vglClLehmerRNG: Gera dois valores aleatórios diferentes (randValX, randValY)
 *                  usando duas sementes diferentes e escreve em .x e .y do pixel.
 *                  Assim, cada pixel terá (x=U1, y=U2, z=0, w=1).
 *                  
 * Parâmetros:
 *   out_img -> imagem de saída
 *   seed1, seed2 -> sementes para cada canal
 */
void vglClLehmerRNG(VglImage* out_img, int seed1, int seed2)
{
  vglCheckContext(out_img, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_VINICIUS/vglClLehmerRNG.cl";
    printf("Compiling %s\n", _file_path);
    std::ifstream _file(_file_path);
    if(_file.fail())
    {
      fprintf(stderr, "%s:%s: Error: File %s not found.\n", __FILE__, __FUNCTION__, _file_path);
      exit(1);
    }
    std::string _prog( std::istreambuf_iterator<char>( _file ), ( std::istreambuf_iterator<char>() ) );
    const char *_source_str = _prog.c_str();
#ifdef __DEBUG__
    printf("Kernel to be compiled:\n%s\n", _source_str);
#endif
    _program = clCreateProgramWithSource(cl.context, 1, (const char **) &_source_str, 0, &_err );
    vglClCheckError(_err, (char*) "clCreateProgramWithSource" );
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_VINICIUS/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClLehmerRNG", &_err );
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &out_img->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( int ), &seed1 );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( int ), &seed2 );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  int _ndim = 2;
  if (out_img->ndim > 2){
    _ndim = 3;
  }

  size_t _worksize_0 = out_img->getWidthIn();
  if (out_img->depth == IPL_DEPTH_1U)
  {
    _worksize_0 = out_img->getWidthStepWords();
  }

  size_t worksize[] = { _worksize_0, out_img->getHeightIn(),  out_img->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(out_img, VGL_CL_CONTEXT);
}

