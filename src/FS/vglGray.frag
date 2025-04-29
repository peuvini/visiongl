/** Convert image to grayscale by calculating the scalar product of (r, g, b) and (.2125, .7154, .0721). */

// (IN_TEX: VglImage* src, OUT_FBO: VglImage* dst)

uniform sampler2D sampler0;

void main(void) {
    vec3 rgb = texture2D(sampler0, gl_TexCoord[0].xy).rgb; // Extrai apenas os componentes RGB
    float gray = dot(rgb, vec3(0.2125, 0.7154, 0.0721));  // Calcula o produto escalar
    gl_FragColor = vec4(gray, gray, gray, 1.0);           // Define a saída em tons de cinza com alpha
}
