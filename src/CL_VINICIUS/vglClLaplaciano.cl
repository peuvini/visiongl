/**
 * Aplica um filtro de aguçamento (sharpen) na imagem de entrada (src)
 * usando o operador Laplaciano. O resultado é armazenado na imagem
 * de saída (dst).
 */
__kernel void vglClLaplaciano(__read_only image2d_t img_input,
                              __write_only image2d_t img_output)
{
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP_TO_EDGE |
                          CLK_FILTER_NEAREST;

    int2 coords = (int2)(get_global_id(0), get_global_id(1));

    // Lê o valor do pixel central e de seus 4 vizinhos diretos.
    float4 center = read_imagef(img_input, smp, coords);
    float4 top    = read_imagef(img_input, smp, (int2)(coords.x, coords.y - 1)); 
    float4 bottom = read_imagef(img_input, smp, (int2)(coords.x, coords.y + 1)); 
    float4 left   = read_imagef(img_input, smp, (int2)(coords.x - 1, coords.y)); 
    float4 right  = read_imagef(img_input, smp, (int2)(coords.x + 1, coords.y)); 

    float4 laplacian_val = (top + bottom + left + right) - (4.0f * center);

    // Aplica o aguçamento: ImagemAguçada = ImagemOriginal - Laplaciano
    float4 sharpened_val = center - laplacian_val;

    write_imagef(img_output, coords, sharpened_val);
}