/** Dilation of image by 3x3 square structuring element.
  */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;
uniform vec2 tex_size; // src->ipl->width, src->ipl->height

void main(void) {
    vec4 pix[9];
    vec4 chosen_val = vec4(0.0);

    // Definir deslocamentos dentro da main
    vec2 offset[9];
    offset[0] = vec2(-1.0, -1.0);
    offset[1] = vec2( 0.0, -1.0);
    offset[2] = vec2( 1.0, -1.0);
    offset[3] = vec2(-1.0,  0.0);
    offset[4] = vec2( 0.0,  0.0);
    offset[5] = vec2( 1.0,  0.0);
    offset[6] = vec2(-1.0,  1.0);
    offset[7] = vec2( 0.0,  1.0);
    offset[8] = vec2( 1.0,  1.0);

    for (int i = 0; i < 9; i++) {
        pix[i] = texture2D(sampler0, gl_TexCoord[0].xy + offset[i] / tex_size);
        chosen_val = max(pix[i], chosen_val);
    }

    gl_FragColor = chosen_val;
}
