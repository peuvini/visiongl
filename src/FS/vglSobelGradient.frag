/** Sobel gradient of image */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;
uniform vec2 tex_size; // src->ipl->width, src->ipl->height

void main(void)
{
    // Declare um array fixo com tamanho explícito
    vec2 offset[9];

    // Atribua valores manualmente (sem inicialização estática)
    offset[0] = vec2(-1.0, -1.0);
    offset[1] = vec2( 0.0, -1.0);
    offset[2] = vec2( 1.0, -1.0);
    offset[3] = vec2(-1.0,  0.0);
    offset[4] = vec2( 0.0,  0.0);
    offset[5] = vec2( 1.0,  0.0);
    offset[6] = vec2(-1.0,  1.0);
    offset[7] = vec2( 0.0,  1.0);
    offset[8] = vec2( 1.0,  1.0);

    vec4 pix[9];

    // Obtenha as cores nos vizinhos, sem usar arrays dinâmicos
    for (int i = 0; i < 9; i++)
    {
        pix[i] = texture2D(sampler0, 
                           gl_TexCoord[0].xy + offset[i] / tex_size);
    }

    // Gradiente de Sobel
    vec4 x = ( -pix[0] + pix[2]
               - 2.0 * pix[3] + 2.0 * pix[5]
               - pix[6] + pix[8] );

    vec4 y = ( -pix[0] - 2.0 * pix[1] - pix[2]
               + pix[6] + 2.0 * pix[7] + pix[8] );

    // Magnitude do gradiente
    gl_FragColor = sqrt(x * x + y * y) / 2.0;
}
