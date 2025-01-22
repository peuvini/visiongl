#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    const char* usage = "\n\
    This program reads a file and saves a copy. Usage is as follows:\n\
\n\
    demo_io <input file> <output file> <option>\n\
    option:\n\
    -1 -> Load image as is.\n\
     0 -> Load image as grayscale.\n\
     1 -> Load image as BGR.\n\
\n";

    if (argc < 4)
    {
        std::cerr << usage;
        return 1;
    }

    const std::string inFilename = argv[1];  // Nome do arquivo de entrada
    const std::string outFilename = argv[2]; // Nome do arquivo de saída
    int option = std::atoi(argv[3]);         // Opção de cor

    // Escolha do modo de carregamento
    int flags = cv::IMREAD_UNCHANGED; // Default: Carrega "as is"
    switch (option)
    {
        case 0: flags = cv::IMREAD_GRAYSCALE; break; // Escala de cinza
        case 1: flags = cv::IMREAD_COLOR;    break;  // BGR
        case -1: break; // IMREAD_UNCHANGED já é o comportamento padrão
        default:
            std::cerr << "Opção inválida! Usando default (Load as is).\n";
            break;
    }

    // Carrega a imagem com base na opção
    cv::Mat image = cv::imread(inFilename, flags);
    if (image.empty())
    {
        std::cerr << "Erro: Não foi possível carregar o arquivo de entrada: " << inFilename << "\n";
        return 1;
    }

    // Salva a imagem no arquivo de saída
    if (!cv::imwrite(outFilename, image))
    {
        std::cerr << "Erro: Não foi possível salvar a imagem no arquivo de saída: " << outFilename << "\n";
        return 1;
    }

    std::cout << "Imagem salva com sucesso no arquivo: " << outFilename << "\n";
    return 0;
}
