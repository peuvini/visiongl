/** Geometric mean of src image. Result is stored in dst image.
*/


__kernel void vglClGeometricMean(__read_only  image2d_t img_input,
                                 __write_only image2d_t img_output)
{
    // Parâmetros do filtro
    const int window_size_x = 7;  // deve ser ímpar
    const int window_size_y = 7;  // deve ser ímpar

    // Coordenadas do pixel atual (global_id(0) -> x, global_id(1) -> y)
    int2 coords = (int2)(get_global_id(0), get_global_id(1));

    // Define o sampler: coordenadas não normalizadas, clamp to edge e sem interpolação
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE   |
                          CLK_FILTER_NEAREST;

    // Cálculo do “meio” da janela
    int half_wx = window_size_x / 2;
    int half_wy = window_size_y / 2;

    // Número de pixels na janela
    float window_area = (float)(window_size_x * window_size_y);

    // Variável para acumular soma dos logs
    float sum_log = 0.0f;

    // Varre a janela ao redor do pixel central
    for (int i = -half_wy; i <= half_wy; i++)
    {
        for (int j = -half_wx; j <= half_wx; j++)
        {
            int2 sample_coords = (int2)(coords.x + j, coords.y + i);

            // Lê valor do pixel (escala de cinza no canal X)
            float4 neighbor = read_imagef(img_input, smp, sample_coords);
            float val = neighbor.x;

            // Evita log(0); se for 0, força um mínimo para não gerar -Inf
            val = fmax(val, 1e-10f);

            // Soma do log do valor do pixel
            sum_log += log(val);
        }
    }

    // Calcula a média dos logs e aplica exp para obter a média geométrica
    float mean_log = sum_log / window_area;
    float geo_mean = exp(mean_log);

    // Como a imagem é em escala de cinza, escrevemos o mesmo valor nos 4 canais
    write_imagef(img_output, coords, (float4)(geo_mean, geo_mean, geo_mean, 1.0f));
}

