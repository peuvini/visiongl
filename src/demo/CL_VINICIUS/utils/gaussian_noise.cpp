// gaussian_noise.cpp  –  gera PGM com ruído gaussiano centrado em 0
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <algorithm>

/*
 * Uso:
 *   ./gaussian_noise [largura altura [stddev]]
 * Ex.:
 *   ./gaussian_noise 512 512 20
 * gera  noise.pgm 512×512 com σ = 20  e média 0 (gravada como 128±20)
 */

 inline int clamp8(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }
 

int main(int argc, char* argv[])
{
    int width  = 512;
    int height = 512;
    double stddev = 20.0;      // desvio-padrão do ruído

    if (argc >= 3) {
        width  = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }
    if (argc >= 4) {
        stddev = std::stod(argv[3]);
    }

    std::mt19937 gen(
        std::chrono::high_resolution_clock::now()
            .time_since_epoch().count());
    std::normal_distribution<double> dist(0.0, stddev);      // média 0!

    std::ofstream ofs("noise.pgm", std::ios::binary);
    if (!ofs) {
        std::cerr << "Erro ao criar noise.pgm\n";
        return 1;
    }

    ofs << "P5\n" << width << ' ' << height << "\n255\n";

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            double n  = dist(gen);       // ruído ∈ (-∞,∞)
            int val   = static_cast<int>(std::round(128.0 + n));
            val = clamp8(val);   // garante faixa PGM
            unsigned char uc = static_cast<unsigned char>(val);
            ofs.write(reinterpret_cast<char*>(&uc), 1);
        }

    ofs.close();
    std::cout << "Imagem gerada: noise.pgm (" << width << 'x' << height
              << "), média≈128, desvio=" << stddev << '\n';
    return 0;
}
