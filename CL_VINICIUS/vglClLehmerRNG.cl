/** 
 * vglClLehmerRNG2: Gera dois valores aleatórios diferentes (randValX, randValY)
 *                  usando duas sementes diferentes e escreve em .x e .y do pixel.
 *                  Assim, cada pixel terá (x=U1, y=U2, z=0, w=1).
 *                  
 * Parâmetros:
 *   out_img -> imagem de saída
 *   seed1, seed2 -> sementes para cada canal
 */
//SCALAR seed1
//SCALAR seed2
__kernel void vglClLehmerRNG(__write_only image2d_t out_img,
                              int seed1,
                              int seed2)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    int width  = get_image_width(out_img);
    int height = get_image_height(out_img);
    if (x >= width || y >= height) return;

    int i = y * width + x;

    const unsigned int a = 48271u;
    const unsigned int m = 2147483647u; // 2^31 - 1

    // Gera valor para canal X (U1)
    unsigned int valueX = (seed1 + i) % m;
    valueX = (valueX * a) % m;
    float randValX = (float)valueX / (float)m;

    // Gera valor para canal Y (U2)
    unsigned int valueY = (seed2 + i) % m;
    valueY = (valueY * a) % m;
    float randValY = (float)valueY / (float)m;

    // Armazena em (x=U1, y=U2, z=0, w=1)
    float4 outColor = (float4)(randValX, randValY, 0.0f, 1.0f);
    write_imagef(out_img, (int2)(x, y), outColor);
}