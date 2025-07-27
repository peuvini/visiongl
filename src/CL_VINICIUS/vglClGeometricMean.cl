/** Geometric mean of src image. Result is stored in dst image.
*/

//SCALAR win
__kernel void vglClGeometricMean(__read_only  image2d_t img_input,
                                 __write_only image2d_t img_output,
                                 int win)
{

    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE   |
                          CLK_FILTER_NEAREST;

    int   rad        = win >> 1;             
    float winAreaInv = 1.0f / (float)(win * win);

    float sumLog = 0.0f;
    for (int dy = -rad; dy <=  rad; ++dy)
        for (int dx = -rad; dx <=  rad; ++dx)
        {
            float pix = read_imagef(img_input, smp,
                                    coord + (int2)(dx, dy)).x;
            pix = fmax(pix, 1e-6f);
            sumLog += log(pix);
        }

    float gmean = exp(sumLog * winAreaInv);
    write_imagef(img_output, coord, (float4)(gmean, gmean, gmean, 1.0f));
}
