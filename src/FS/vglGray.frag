/** Convert image to grayscale by calculating
 *  the scalar product of (r, g, b) and (0.2125, 0.7154, 0.0721).
 *
 *  (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)
 */

uniform sampler2D sampler0;

void main(void)
{
    // Lê a cor original no ponto (x, y)
    vec4 color = texture2D(sampler0, gl_TexCoord[0].xy);

    // Calcula o valor em escala de cinza usando apenas as componentes RGB
    float gray = dot(vec3(0.2125, 0.7154, 0.0721), color.rgb);

    // Define a cor de saída como (gray, gray, gray, alpha_original)
    gl_FragColor = vec4(vec3(gray), color.a);
}
