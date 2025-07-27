/*--------------------------------------------------------------------------*
 * Harmonic mean of src image. Result is stored in dst image.
 */

// SCALAR win
__kernel void vglClHarmonicMean(__read_only  image2d_t img_input,
                                __write_only image2d_t img_output,
                                int          win)
{

    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP          |
                          CLK_FILTER_NEAREST;

    int   rad     = win >> 1;
    int   cntPix  = win * win;                
    const float eps = 1.0f / 255.0f;          

    
    float invSum = 0.0f;                      

    for (int dy = -rad; dy <= rad; ++dy)
        for (int dx = -rad; dx <= rad; ++dx)
        {
            
            float g = read_imagef(img_input, smp, coord + (int2)(dx, dy)).x;
            g = fmax(g, eps);
            invSum += 1.0f / g;
        }

    float hmean = ((float)cntPix) / invSum;   // (m·n) / Σ 1/g

    
    float4 outPix = (float4)(hmean, hmean, hmean, 1.0f);
    write_imagef(img_output, coord, outPix);
}
