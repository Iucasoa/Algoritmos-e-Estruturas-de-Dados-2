#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "arvore_huffman.h"
#include "utilitarios.h"

/* Descomprime .huff gerado pelo compressor acima. */
int descompressor_descomprimir_arquivo(const char* entrada, const char* saida) {
    long tam = arquivo_obter_tamanho(entrada);
    if (tam < 0) {
        fprintf(stderr, "Erro: não foi possível abrir %s\n", entrada);
        return 1;
    }
    if (tam == 0) {
        fprintf(stderr, "Arquivo vazio: nada a descomprimir\n");
        return 1;
    }
    unsigned char* buf = (unsigned char*)malloc(tam);
    if (!buf) return 1;
    int lidos = arquivo_ler_tudo(entrada, buf, (int)tam);
    if (lidos != tam) { free(buf); return 1; }

    const unsigned char* p = buf;
    uint64_t original_size = 0;
    memcpy(&original_size, p, sizeof(original_size));
    p += sizeof(original_size);
    uint32_t tree_size = 0;
    memcpy(&tree_size, p, sizeof(tree_size));
    p += sizeof(tree_size);

    if (tree_size > (uint32_t)(tam - (sizeof(original_size) + sizeof(tree_size)))) {
        free(buf);
        fprintf(stderr, "Erro: tamanho de árvore inválido\n");
        return 1;
    }

    int consumidos = 0;
    NoHuffman* raiz = arvorehuffman_desserializar(p, (int)tree_size, &consumidos);
    if (!raiz && original_size > 0) {
        free(buf);
        fprintf(stderr, "Erro: falha ao desserializar árvore\n");
        return 1;
    }
    p += tree_size;
    int bitstream_size = (int)(tam - (p - buf));
    const unsigned char* bitstream = p;

    /* caso especial: árvore nula ou arquivo original vazio */
    if (!raiz) {
        /* se original_size == 0, criar arquivo vazio */
        FILE* out = fopen(saida, "wb");
        if (!out) { free(buf); return 1; }
        fclose(out);
        free(buf);
        return 0;
    }

    /* caso: apenas um símbolo na árvore (folha única) */
    if (nohuffman_eh_folha(raiz)) {
        FILE* out = fopen(saida, "wb");
        if (!out) { arvorehuffman_destruir(raiz); free(buf); return 1; }
        for (uint64_t i = 0; i < original_size; ++i) {
            fwrite(&raiz->simbolo, 1, 1, out);
        }
        fclose(out);
        arvorehuffman_destruir(raiz);
        free(buf);
        return 0;
    }

    LeitorBits* leitor = leitorbits_criar(bitstream, bitstream_size);
    if (!leitor) { arvorehuffman_destruir(raiz); free(buf); return 1; }

    FILE* out = fopen(saida, "wb");
    if (!out) { leitorbits_destruir(leitor); arvorehuffman_destruir(raiz); free(buf); return 1; }

    uint64_t escritos = 0;
    while (escritos < original_size) {
        NoHuffman* cur = raiz;
        while (!nohuffman_eh_folha(cur)) {
            int bit = leitorbits_ler_bit(leitor);
            if (bit < 0) { /* fim inesperado do bitstream */
                break;
            }
            if (bit == 0) cur = cur->esquerda;
            else cur = cur->direita;
            if (!cur) break;
        }
        if (!cur) break;
        unsigned char simb = cur->simbolo;
        fwrite(&simb, 1, 1, out);
        escritos++;
    }

    fclose(out);
    leitorbits_destruir(leitor);
    arvorehuffman_destruir(raiz);
    free(buf);

    if (escritos != original_size) {
        fprintf(stderr, "Aviso: esperado %llu bytes mas foram escritos %llu.\n",
                (unsigned long long)original_size, (unsigned long long)escritos);
        return 1;
    }
    return 0;
}
