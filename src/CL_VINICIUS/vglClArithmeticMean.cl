/** Arithmetic-mean filter.
 *
 *  Para cada pixel (x,y) calcula  
 *      f̂(x,y) = 1/(m·n) · Σ g(s,t) , (s,t) ∈ janela m×n
 *
 *  - `win`  = tamanho da janela (m = n = win) - deve ser ímpar  
 *  - Usa endereço `CLK_ADDRESS_CLAMP` → replica o valor da borda  
 *  - Processa imagens RGBA (UNORM_INT8 ou FLOAT); acumula em `float4`
 */
//SCALAR win
__kernel void vglClArithmeticMean(__read_only  image2d_t img_input,
                                  __write_only image2d_t img_output,
                                int    win)
{
    /*----------------------------------*
     * 1. Coords e sampler              *
     *----------------------------------*/
    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |   // coordenadas em pixels
                          CLK_ADDRESS_CLAMP          |   // padding por repetição
                          CLK_FILTER_NEAREST;            // sem interpolação

    /*----------------------------------*
     * 2. Acumula a soma da janela      *
     *----------------------------------*/
    float4 acc  = (float4)(0.0f);
    int     rad = win >> 1;              // win = 2·rad + 1

    for (int dy = -rad; dy <= rad; ++dy)
        for (int dx = -rad; dx <= rad; ++dx)
            acc += read_imagef(img_input, smp, coord + (int2)(dx, dy));

    /*----------------------------------*
     * 3. Normaliza e grava             *
     *----------------------------------*/
    float norm = 1.0f / (float)(win * win);
    write_imagef(img_output, coord, acc * norm);
}
