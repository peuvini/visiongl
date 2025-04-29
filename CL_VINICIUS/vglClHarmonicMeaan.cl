/** Harmonic mean of src image. Result is stored in dst image.
 *
 *  f̂(x,y) = (m·n) / Σ [ 1 / g(s,t) ]
 *  where m = window_size_x, n = window_size_y
 */

__kernel void vglClHarmonicMean(__read_only  image2d_t img_input,
                                __write_only image2d_t img_output)
{
    // Parâmetros do filtro (devem ser ímpares)
    const int window_size_x = 3;
    const int window_size_y = 3;

    // Coordenadas do pixel atual
    int2 coords = (int2)(get_global_id(0), get_global_id(1));

    // Sampler: coordenadas não normalizadas, clamp-to-edge, sem interpolação
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE   |
                          CLK_FILTER_NEAREST;

    // Metades das janelas
    int half_wx = window_size_x / 2;
    int half_wy = window_size_y / 2;

    // Número total de pixels na janela
    float window_area = (float)(window_size_x * window_size_y);

    // Acumulador da soma dos recíprocos
    float sum_recip = 0.0f;

    // Varre a vizinhança
    for (int dy = -half_wy; dy <= half_wy; dy++) {
        for (int dx = -half_wx; dx <= half_wx; dx++) {
            int2 sample_coords = (int2)(coords.x + dx, coords.y + dy);

            // Lê o pixel em grayscale (canal .x)
            float4 pix = read_imagef(img_input, smp, sample_coords);
            float val = pix.x;

            // Evita divisão por zero
            val = fmax(val, 1e-6f);

            // Soma do recíproco
            sum_recip += 1.0f / val;
        }
    }

    // Média harmônica: área da janela / soma dos recíprocos
    float harm_mean = window_area / sum_recip;

    // Escreve em todos os canais (RGBA)
    write_imagef(img_output, coords, (float4)(harm_mean, harm_mean, harm_mean, 1.0f));
}
