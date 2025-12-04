#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "arvore_huffman.h"
#include "utilitarios.h"

/* Compacta arquivo entrada -> saida (.huff)
   Formato:
   - uint64_t original_size (8 bytes)
   - uint32_t tree_size (4 bytes)
   - tree_bytes (pré-ordem): 0x01 <simbolo> para folha, 0x00 para interno
   - bitstream (bytes)
*/
int compressor_comprimir_arquivo(const char* entrada, const char* saida) {
    long tam = arquivo_obter_tamanho(entrada);
    if (tam < 0) {
        fprintf(stderr, "Erro: não foi possível abrir %s\n", entrada);
        return 1;
    }
    if (tam == 0) {
        /* criar .huff minimal */
        FILE* f = fopen(saida, "wb");
        if (!f) return 1;
        uint64_t original = 0;
        uint32_t tree_size = 0;
        fwrite(&original, sizeof(original), 1, f);
        fwrite(&tree_size, sizeof(tree_size), 1, f);
        fclose(f);
        return 0;
    }

    unsigned char* dados = (unsigned char*)malloc(tam);
    if (!dados) return 1;
    int lidos = arquivo_ler_tudo(entrada, dados, (int)tam);
    if (lidos != tam) { free(dados); return 1; }

    int freq[256] = {0};
    for (long i = 0; i < tam; ++i) freq[dados[i]]++;

    NoHuffman* raiz = arvorehuffman_construir(freq);
    if (!raiz) { free(dados); return 1; }

    TabelaHuffman tabela;
    arvorehuffman_gerar_tabela(raiz, &tabela);

    /* serializar árvore */
    int max_tree = 2048;
    unsigned char* tree_buf = (unsigned char*)malloc(max_tree);
    if (!tree_buf) { arvorehuffman_destruir(raiz); free(dados); return 1; }
    int tree_bytes = arvorehuffman_serializar(raiz, tree_buf, max_tree);
    if (tree_bytes < 0) {
        /* se buffer insuficiente, realocar maior e tentar novamente */
        free(tree_buf);
        max_tree = 8192;
        tree_buf = (unsigned char*)malloc(max_tree);
        if (!tree_buf) { arvorehuffman_destruir(raiz); free(dados); return 1; }
        tree_bytes = arvorehuffman_serializar(raiz, tree_buf, max_tree);
        if (tree_bytes < 0) { free(tree_buf); arvorehuffman_destruir(raiz); free(dados); return 1; }
    }

    /* estimativa para output: cabeçalho + bitstream (pior caso = tamanho original) */
    int header_est = (int)(8 + 4 + tree_bytes);
    int max_out = header_est + (int)tam + 16;
    unsigned char* out_buf = (unsigned char*)malloc(max_out);
    if (!out_buf) { free(tree_buf); arvorehuffman_destruir(raiz); free(dados); return 1; }

    /* reservar cabeçalho */
    memset(out_buf, 0, header_est);
    unsigned char* p = out_buf + header_est;
    int restante = max_out - header_est;
    EscritorBits* escritor = escritorbits_criar(p, restante);
    if (!escritor) { free(out_buf); free(tree_buf); arvorehuffman_destruir(raiz); free(dados); return 1; }

    for (long i = 0; i < tam; ++i) {
        unsigned char simb = dados[i];
        if (tabela.codigo[simb]) {
            escritorbits_escrever_codigo(escritor, tabela.codigo[simb]);
        } else {
            /* não deveria ocorrer */
        }
    }
    int bytes_bitstream = escritorbits_finalizar(escritor);
    escritorbits_destruir(escritor);

    /* preencher cabeçalho no início do buffer */
    unsigned char* cursor = out_buf;
    uint64_t original_size = (uint64_t)tam;
    memcpy(cursor, &original_size, sizeof(original_size));
    cursor += sizeof(original_size);
    uint32_t tree_size32 = (uint32_t)tree_bytes;
    memcpy(cursor, &tree_size32, sizeof(tree_size32));
    cursor += sizeof(tree_size32);
    memcpy(cursor, tree_buf, tree_bytes);

    int total_written = header_est + bytes_bitstream;
    if (arquivo_escrever_tudo(saida, out_buf, total_written) != 0) {
        fprintf(stderr, "Erro: não foi possível escrever %s\n", saida);
        /* libera tudo */
        for (int i = 0; i < 256; ++i) if (tabela.codigo[i]) free(tabela.codigo[i]);
        arvorehuffman_destruir(raiz);
        free(out_buf); free(tree_buf); free(dados);
        return 1;
    }

    imprimir_estatisticas(tam, total_written);

    /* libera */
    for (int i = 0; i < 256; ++i) if (tabela.codigo[i]) free(tabela.codigo[i]);
    arvorehuffman_destruir(raiz);
    free(out_buf);
    free(tree_buf);
    free(dados);
    return 0;
}

