/** Dilation of image by 3x3 square structuring element. */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;
uniform vec2 tex_size; // src->ipl->width, src->ipl->height

void main(void)
{
    // Declare array com tamanho fixo sem 'const'.
    vec2 offset[9];

    // Atribui cada elemento manualmente (sem construtor de array).
    offset[0] = vec2(-1.0, -1.0);
    offset[1] = vec2( 0.0, -1.0);
    offset[2] = vec2( 1.0, -1.0);
    offset[3] = vec2(-1.0,  0.0);
    offset[4] = vec2( 0.0,  0.0);
    offset[5] = vec2( 1.0,  0.0);
    offset[6] = vec2(-1.0,  1.0);
    offset[7] = vec2( 0.0,  1.0);
    offset[8] = vec2( 1.0,  1.0);

    vec4 chosen_val = vec4(0.0);

    // Loop sobre os 9 offsets
    for (int i = 0; i < 9; i++)
    {
        vec4 pix = texture2D(sampler0, 
                             gl_TexCoord[0].xy + offset[i] / tex_size);
        chosen_val = max(pix, chosen_val);
    }

    gl_FragColor = chosen_val;
}
