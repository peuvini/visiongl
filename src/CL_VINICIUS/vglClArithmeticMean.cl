/** Arithmetic mean of src image. Result is stored in dst image.
 */
__kernel void vglClArithmeticMean(__read_only  image2d_t img_input,
                                  __write_only image2d_t img_output,
                                  int          win)
{
    /* 1. Coordenadas do pixel atual */
    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    /*  sampler: coords em pixels | clamp na borda | sem interpolação */
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP        | 
                          CLK_FILTER_NEAREST;

    /* 2. Soma da janela */
    float4 acc = (float4)(0.0f);
    int rad     = win >> 1;          
    int winArea = win * win;         

    for (int dy = -rad; dy <= rad; ++dy)
        for (int dx = -rad; dx <= rad; ++dx)
            acc += read_imagef(img_input, smp, coord + (int2)(dx, dy));

    /* 3. Normaliza e grava */
    float norm = 1.0f / (float)winArea;
    write_imagef(img_output, coord, acc * norm);
}
