/** vglClGaussianNoise
 *
 *  Adiciona ruído gaussiano N(μ, σ²) pixel-a-pixel:
 *      out(x,y) = clamp( in(x,y) + μ + σ·Z , 0 , 1 )
 *  onde Z é obtido pelo método de Box–Muller a partir de dois
 *  números U ~ U(0,1) gerados por um LCG simples (Park-Miller).
 *
 *  Parâmetros
 *      img_input   – imagem de entrada  (UNORM_INT8 ou FLOAT [0-1])
 *      img_output  – imagem de saída    (mesmo formato)
 *      seed        – semente global (mude em cada execução se quiser
 *                    ruídos diferentes)
 *      mean        – μ   (ex.: 0.0f)
 *      sigma       – σ   (ex.: 0.1f → variância 0.01)
 *
 *  O kernel produz **um** número gaussiano por pixel e o aplica a
 *  todos os canais (RGBA).  Para canais independentes basta gerar
 *  quatro amostras em vez de uma.
 */
//SCALAR seed
//SCALAR mean
//SCALAR sigma
__kernel void vglClGaussianNoise(__read_only  image2d_t img_input,
                                 __write_only image2d_t img_output,
                                 uint  seed,
                                 float mean,
                                 float sigma)
{
    /*----------------------------------*
     * 1. Coords e sampler              *
     *----------------------------------*/
    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE |
                          CLK_ADDRESS_CLAMP          |
                          CLK_FILTER_NEAREST;

    /*----------------------------------*
     * 2. Lê pixel original             *
     *----------------------------------*/
    float4 pix = read_imagef(img_input, smp, coord);

    /*----------------------------------*
     * 3. Gera N(0,1) via Box–Muller     *
     *----------------------------------*/
    /* 3.1  LCG (Park–Miller) ---------- */
    const uint A = 48271U;
    const uint M = 2147483647U;      // 2³¹-1, primo

    // semente distinta por pixel
    uint s = (uint)coord.x * 73856093U ^ (uint)coord.y * 19349663U ^ seed;
    s = (s ? s : 1U);                // evita zero

    // gera U1, U2 em (0,1)
    s = (uint)((ulong)A * s % M);
    float U1 = (float)s / (float)M;

    s = (uint)((ulong)A * s % M);
    float U2 = (float)s / (float)M;

    U1 = fmax(U1, 1e-7f);            // evita log(0)
    const float PI = 3.14159265358979f;
    float R = sqrt(-2.0f * log(U1));
    float Z = R * cos(2.0f * PI * U2);   // amostra gaussiana

    /*----------------------------------*
     * 4. Aplica ruído e grava          *
     *----------------------------------*/
    float noise = mean + sigma * Z;
    float4 out  = clamp(pix + (float4)(noise), 0.0f, 1.0f);

    write_imagef(img_output, coord, out);
}
