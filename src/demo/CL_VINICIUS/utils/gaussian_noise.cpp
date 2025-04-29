// gaussian_noise.cpp
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <algorithm>

int main(int argc, char* argv[]) {
    // Parâmetros padrão
    int width  = 512;
    int height = 512;
    double mean   = 128.0;  // média do ruído
    double stddev = 20.0;   // desvio-padrão do ruído

    // Permite passar largura e altura pela linha de comando:
    // ./gaussian_noise 512 512  (usa mean/stddev padrão)
    if (argc >= 3) {
        width  = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }
    // Permite também customizar média e desvio:
    // ./gaussian_noise 512 512 100 30
    if (argc >= 5) {
        mean   = std::stod(argv[3]);
        stddev = std::stod(argv[4]);
    }

    // Inicializa gerador de números pseudo‐aleatórios
    std::mt19937 gen(
        std::chrono::high_resolution_clock::now()
            .time_since_epoch().count()
    );
    std::normal_distribution<double> dist(mean, stddev);

    // Abre arquivo de saída (binário) no mesmo diretório do executável
    std::ofstream ofs("noise.pgm", std::ios::binary);
    if (!ofs) {
        std::cerr << "Erro ao criar arquivo noise.pgm\n";
        return 1;
    }

    // Escreve cabeçalho PGM (P5 = binário, max valor 255)
    ofs << "P5\n" 
        << width << " " << height << "\n"
        << "255\n";

    // Gera pixel a pixel
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int v = static_cast<int>(std::round(dist(gen)));
            v = std::max(0, std::min(255, v));  // limita [0,255]
            unsigned char uc = static_cast<unsigned char>(v);
            ofs.write(reinterpret_cast<char*>(&uc), 1);
        }
    }

    ofs.close();
    std::cout << "Imagem gerada: noise.pgm (" 
              << width << "x" << height << ")\n";
    return 0;
}
