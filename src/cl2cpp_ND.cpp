
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

/** N-dimensional convolution

    SHAPE directive passes a structure with size of each dimension, offsets and number of dimensions. Parameter does not appear in wrapper parameter list. The C expression between parenthesis returns the desired shape of type VglClShape.
    
  */
void vglClNdConvolution(VglImage* img_input, VglImage* img_output, VglStrEl* window)
{
  if (  ( (img_input->ndim == 2) || (img_input->ndim == 3) )  &&  !(img_input->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_input.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  if (  ( (img_output->ndim == 2) || (img_output->ndim == 3) )  &&  !(img_output->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_output.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  cl_mem mobj_img_shape = NULL;
  mobj_img_shape = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(VglClShape), NULL, &_err);
  vglClCheckError( _err, (char*) "clCreateBuffer img_shape" );
  _err = clEnqueueWriteBuffer(cl.commandQueue, mobj_img_shape, CL_TRUE, 0, sizeof(VglClShape), img_input->vglShape->asVglClShape(), 0, NULL, NULL);
  vglClCheckError( _err, (char*) "clEnqueueWriteBuffer img_shape" );

  cl_mem mobj_window = NULL;
  mobj_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(VglClStrEl), NULL, &_err);
  vglClCheckError( _err, (char*) "clCreateBuffer window" );
  _err = clEnqueueWriteBuffer(cl.commandQueue, mobj_window, CL_TRUE, 0, sizeof(VglClStrEl), window->asVglClStrEl(), 0, NULL, NULL);
  vglClCheckError( _err, (char*) "clEnqueueWriteBuffer window" );

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_ND/vglClNdConvolution.cl";
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
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_ND/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClNdConvolution", &_err ); 
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( cl_mem ), (void*) &mobj_img_shape );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  _err = clSetKernelArg( _kernel, 3, sizeof( cl_mem ), (void*) &mobj_window );
  vglClCheckError( _err, (char*) "clSetKernelArg 3" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }
  size_t worksize[] = { img_input->getWidthIn(), img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  _err = clReleaseMemObject( mobj_img_shape );
  vglClCheckError(_err, (char*) "clReleaseMemObject mobj_img_shape");

  _err = clReleaseMemObject( mobj_window );
  vglClCheckError(_err, (char*) "clReleaseMemObject mobj_window");

  vglSetContext(img_input, VGL_CL_CONTEXT);

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Copy N-dimensional image.

  */
void vglClNdCopy(VglImage* img_input, VglImage* img_output)
{
  if (  ( (img_input->ndim == 2) || (img_input->ndim == 3) )  &&  !(img_input->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_input.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  if (  ( (img_output->ndim == 2) || (img_output->ndim == 3) )  &&  !(img_output->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_output.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_ND/vglClNdCopy.cl";
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
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_ND/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClNdCopy", &_err ); 
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
  size_t worksize[] = { img_input->getWidthIn(), img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_input, VGL_CL_CONTEXT);

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** N-dimensional dilation

    SHAPE directive passes a structure with size of each dimension, offsets and number of dimensions. Parameter does not appear in wrapper parameter list. The C expression between parenthesis returns the desired shape of type VglClShape.
    
  */
void vglClNdDilate(VglImage* img_input, VglImage* img_output, VglStrEl* window)
{
  if (  ( (img_input->ndim == 2) || (img_input->ndim == 3) )  &&  !(img_input->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_input.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  if (  ( (img_output->ndim == 2) || (img_output->ndim == 3) )  &&  !(img_output->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_output.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  cl_mem mobj_img_shape = NULL;
  mobj_img_shape = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(VglClShape), NULL, &_err);
  vglClCheckError( _err, (char*) "clCreateBuffer img_shape" );
  _err = clEnqueueWriteBuffer(cl.commandQueue, mobj_img_shape, CL_TRUE, 0, sizeof(VglClShape), img_input->vglShape->asVglClShape(), 0, NULL, NULL);
  vglClCheckError( _err, (char*) "clEnqueueWriteBuffer img_shape" );

  cl_mem mobj_window = NULL;
  mobj_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(VglClStrEl), NULL, &_err);
  vglClCheckError( _err, (char*) "clCreateBuffer window" );
  _err = clEnqueueWriteBuffer(cl.commandQueue, mobj_window, CL_TRUE, 0, sizeof(VglClStrEl), window->asVglClStrEl(), 0, NULL, NULL);
  vglClCheckError( _err, (char*) "clEnqueueWriteBuffer window" );

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_ND/vglClNdDilate.cl";
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
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_ND/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClNdDilate", &_err ); 
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( cl_mem ), (void*) &mobj_img_shape );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  _err = clSetKernelArg( _kernel, 3, sizeof( cl_mem ), (void*) &mobj_window );
  vglClCheckError( _err, (char*) "clSetKernelArg 3" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }
  size_t worksize[] = { img_input->getWidthIn(), img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  _err = clReleaseMemObject( mobj_img_shape );
  vglClCheckError(_err, (char*) "clReleaseMemObject mobj_img_shape");

  _err = clReleaseMemObject( mobj_window );
  vglClCheckError(_err, (char*) "clReleaseMemObject mobj_window");

  vglSetContext(img_input, VGL_CL_CONTEXT);

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** N-dimensional erosion

    SHAPE directive passes a structure with size of each dimension, offsets and number of dimensions. Parameter does not appear in wrapper parameter list. The C expression between parenthesis returns the desired shape of type VglClShape.
    
  */
void vglClNdErode(VglImage* img_input, VglImage* img_output, VglStrEl* window)
{
  if (  ( (img_input->ndim == 2) || (img_input->ndim == 3) )  &&  !(img_input->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_input.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  if (  ( (img_output->ndim == 2) || (img_output->ndim == 3) )  &&  !(img_output->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_output.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  cl_mem mobj_img_shape = NULL;
  mobj_img_shape = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(VglClShape), NULL, &_err);
  vglClCheckError( _err, (char*) "clCreateBuffer img_shape" );
  _err = clEnqueueWriteBuffer(cl.commandQueue, mobj_img_shape, CL_TRUE, 0, sizeof(VglClShape), img_input->vglShape->asVglClShape(), 0, NULL, NULL);
  vglClCheckError( _err, (char*) "clEnqueueWriteBuffer img_shape" );

  cl_mem mobj_window = NULL;
  mobj_window = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(VglClStrEl), NULL, &_err);
  vglClCheckError( _err, (char*) "clCreateBuffer window" );
  _err = clEnqueueWriteBuffer(cl.commandQueue, mobj_window, CL_TRUE, 0, sizeof(VglClStrEl), window->asVglClStrEl(), 0, NULL, NULL);
  vglClCheckError( _err, (char*) "clEnqueueWriteBuffer window" );

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_ND/vglClNdErode.cl";
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
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_ND/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClNdErode", &_err ); 
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( cl_mem ), (void*) &mobj_img_shape );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  _err = clSetKernelArg( _kernel, 3, sizeof( cl_mem ), (void*) &mobj_window );
  vglClCheckError( _err, (char*) "clSetKernelArg 3" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }
  size_t worksize[] = { img_input->getWidthIn(), img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  _err = clReleaseMemObject( mobj_img_shape );
  vglClCheckError(_err, (char*) "clReleaseMemObject mobj_img_shape");

  _err = clReleaseMemObject( mobj_window );
  vglClCheckError(_err, (char*) "clReleaseMemObject mobj_window");

  vglSetContext(img_input, VGL_CL_CONTEXT);

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Invert N-dimensional image.

  */
void vglClNdNot(VglImage* img_input, VglImage* img_output)
{
  if (  ( (img_input->ndim == 2) || (img_input->ndim == 3) )  &&  !(img_input->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_input.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  if (  ( (img_output->ndim == 2) || (img_output->ndim == 3) )  &&  !(img_output->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_output.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_ND/vglClNdNot.cl";
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
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_ND/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClNdNot", &_err ); 
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
  size_t worksize[] = { img_input->getWidthIn(), img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_input, VGL_CL_CONTEXT);

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

/** Threshold of img_input by parameter. if the pixel is below thresh,
    the output is 0, else, the output is top. Result is stored in img_output.
  */
void vglClNdThreshold(VglImage* img_input, VglImage* img_output, unsigned char thresh, unsigned char top)
{
  if (  ( (img_input->ndim == 2) || (img_input->ndim == 3) )  &&  !(img_input->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_input.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  if (  ( (img_output->ndim == 2) || (img_output->ndim == 3) )  &&  !(img_output->clForceAsBuf)  )
  {
    fprintf(stderr, "%s: %s: Error: this operator supports only data as buffer. Please call vglClForceAsBuf() just after creating img_output.\n", __FILE__, __FUNCTION__);
    exit(1);
  }
  vglCheckContext(img_input, VGL_CL_CONTEXT);
  vglCheckContext(img_output, VGL_CL_CONTEXT);

  cl_int _err;

  static cl_program _program = NULL;
  if (_program == NULL)
  {
    char* _file_path = (char*) "CL_ND/vglClNdThreshold.cl";
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
    _err = clBuildProgram(_program, 1, cl.deviceId, "-I CL_ND/", NULL, NULL );
    vglClBuildDebug(_err, _program);
  }

  static cl_kernel _kernel = NULL;
  if (_kernel == NULL)
  {
    _kernel = clCreateKernel( _program, "vglClNdThreshold", &_err ); 
    vglClCheckError(_err, (char*) "clCreateKernel" );
  }


  _err = clSetKernelArg( _kernel, 0, sizeof( cl_mem ), (void*) &img_input->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 0" );

  _err = clSetKernelArg( _kernel, 1, sizeof( cl_mem ), (void*) &img_output->oclPtr );
  vglClCheckError( _err, (char*) "clSetKernelArg 1" );

  _err = clSetKernelArg( _kernel, 2, sizeof( unsigned char ), &thresh );
  vglClCheckError( _err, (char*) "clSetKernelArg 2" );

  _err = clSetKernelArg( _kernel, 3, sizeof( unsigned char ), &top );
  vglClCheckError( _err, (char*) "clSetKernelArg 3" );

  int _ndim = 2;
  if (img_input->ndim > 2){
    _ndim = 3;
  }
  size_t worksize[] = { img_input->getWidthIn(), img_input->getHeightIn(),  img_input->getNFrames() };
  clEnqueueNDRangeKernel( cl.commandQueue, _kernel, _ndim, NULL, worksize, 0, 0, 0, 0 );

  vglClCheckError( _err, (char*) "clEnqueueNDRangeKernel" );

  vglSetContext(img_input, VGL_CL_CONTEXT);

  vglSetContext(img_output, VGL_CL_CONTEXT);
}

