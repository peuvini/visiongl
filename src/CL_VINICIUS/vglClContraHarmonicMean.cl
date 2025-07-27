/* Contraharmonic mean filter of src image. Result is stored in dst image
*/

// SCALAR win
// SCALAR q

__kernel void vglClContraHarmonicMean(__read_only image2d_t img_input,
                                      __write_only image2d_t img_output,
                                      int win,
                                      float q
                                      )
{

        int2 coord = (int2)(get_global_id(0), get_global_id(1));


        const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                              CLK_ADDRESS_CLAMP |
                              CLK_FILTER_NEAREST;

        int rad = win >> 1; // win = 2·rad + 1

        const float eps = 1.0f/255.0f; // evita 0^q quando q<0
        float numSum = 0.0f; // Σ g^{q+1}

        float denSum = 0.0f; // Σ g^{q}

        for (int dy = -rad; dy <= rad; ++dy)
        for (int dx = -rad; dx <= rad; ++dx)
            {

            float g = read_imagef(img_input, smp, coord + (int2)(dx,dy)).x;

            if (q < 0.0f) // ruído-sal: evita 0^neg
                g = fmax(g, eps); //1/0 se torna eps

                float g_q = powr(g, q);

                float g_q1 = g_q * g; // g^{q+1}
                numSum += g_q1;
                denSum += g_q;
            }

        float f = (denSum > 0.0f) ? (numSum / denSum) : 0.0f;
        write_imagef(img_output, coord, (float4)(f, f, f, 1.0f));
} 