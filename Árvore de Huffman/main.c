#include <stdio.h>
#include <string.h>

int compressor_comprimir_arquivo(const char* entrada, const char* saida);
int descompressor_descomprimir_arquivo(const char* entrada, const char* saida);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Uso: %s <comprimir|descomprimir> <entrada> <saida>\n", argv[0]);
        return 1;
    }
    const char* comando = argv[1];
    const char* entrada = argv[2];
    const char* saida = argv[3];

    if (strcmp(comando, "comprimir") == 0) {
        printf("Comprimindo %s -> %s\n", entrada, saida);
        return compressor_comprimir_arquivo(entrada, saida);
    } else if (strcmp(comando, "descomprimir") == 0) {
        printf("Descomprimindo %s -> %s\n", entrada, saida);
        return descompressor_descomprimir_arquivo(entrada, saida);
    } else {
        printf("Comando inválido: %s\n", comando);
        return 1;
    }
}
