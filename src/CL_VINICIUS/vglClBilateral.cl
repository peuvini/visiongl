/** 
 * Filtro Bilateral: aplica ponderação espacial e de similaridade (range) para suavização preservando bordas.
 * A função lê a imagem de entrada e escreve a imagem filtrada na saída.
 * Os parâmetros sigma_spatial, sigma_range, window_size_x e window_size_y foram fixados.
 */
__kernel void vglClBilateral(__read_only image2d_t img_input,
                             __write_only image2d_t img_output)
{
    // Parâmetros fixos do filtro
    const float sigma_spatial = 5.0f;   
    const float sigma_range   = 0.1f;   
    const int window_size_x   = 7;      // deve ser ímpar
    const int window_size_y   = 7;      // deve ser ímpar

    // Coordenadas do pixel atual
    int2 coords = (int2)(get_global_id(0), get_global_id(1));
    
    // Define o sampler: coordenadas não normalizadas, clamp to edge e sem interpolação
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE |
                          CLK_FILTER_NEAREST;
    
    // Lê o valor do pixel central
    float4 center = read_imagef(img_input, smp, coords);
    
    // Inicializa as variáveis de acumulação e normalização
    float4 accum = (float4)(0.0f);
    float norm_factor = 0.0f;

    int half_wx = window_size_x / 2;
    int half_wy = window_size_y / 2;

    // Pré-cálculo dos denominadores para a função gaussiana
    float twoSigmaSpatial2 = 2.0f * sigma_spatial * sigma_spatial;
    float twoSigmaRange2   = 2.0f * sigma_range * sigma_range;

    // Varre a janela ao redor do pixel central
    for (int i = -half_wy; i <= half_wy; i++)
    {
        for (int j = -half_wx; j <= half_wx; j++)
        {
            int2 sample_coords = (int2)(coords.x + j, coords.y + i);
            float4 neighbor = read_imagef(img_input, smp, sample_coords);
    
            // Peso espacial baseado na distância euclidiana
            float spatial_dist2 = (float)(j * j + i * i);
            float weight_spatial = exp(-spatial_dist2 / twoSigmaSpatial2);
            
            // Peso de similaridade (range) baseado na diferença de intensidade
            float diff = length(neighbor - center);
            float weight_range = exp(-(diff * diff) / twoSigmaRange2);

            // Combina os dois pesos
            float weight = weight_spatial * weight_range;

            accum += neighbor * weight;
            norm_factor += weight;
        }
    }
    
    // Normaliza o resultado e escreve na imagem de saída
    float4 result = accum / norm_factor;
    write_imagef(img_output, coords, result);
}
