#include "utilitarios.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Escritor de bits (MSB-first por byte). */
EscritorBits* escritorbits_criar(unsigned char* buffer, int capacidade) {
    if (!buffer || capacidade <= 0) return NULL;
    EscritorBits* e = (EscritorBits*)malloc(sizeof(EscritorBits));
    if (!e) return NULL;
    e->buffer = buffer;
    e->capacidade = capacidade;
    e->pos_byte = 0;
    e->pos_bit = 0;
    e->buffer[0] = 0;
    return e;
}

void escritorbits_escrever_bit(EscritorBits* escritor, int bit) {
    if (!escritor) return;
    if (escritor->pos_byte >= escritor->capacidade) return;
    int shift = 7 - escritor->pos_bit;
    if (bit) escritor->buffer[escritor->pos_byte] |= (1 << shift);
    escritor->pos_bit++;
    if (escritor->pos_bit == 8) {
        escritor->pos_bit = 0;
        escritor->pos_byte++;
        if (escritor->pos_byte < escritor->capacidade) escritor->buffer[escritor->pos_byte] = 0;
    }
}

void escritorbits_escrever_codigo(EscritorBits* escritor, const char* codigo) {
    if (!escritor || !codigo) return;
    for (const char* p = codigo; *p; ++p) {
        escritorbits_escrever_bit(escritor, (*p == '1') ? 1 : 0);
    }
}

/* Retorna bytes usados no buffer (inclui byte parcialmente preenchido). */
int escritorbits_finalizar(EscritorBits* escritor) {
    if (!escritor) return 0;
    int usados = escritor->pos_byte + (escritor->pos_bit > 0 ? 1 : 0);
    return usados;
}

void escritorbits_destruir(EscritorBits* escritor) {
    free(escritor);
}

/* Leitor de bits (MSB-first por byte). */
LeitorBits* leitorbits_criar(const unsigned char* buffer, int tamanho) {
    if (!buffer || tamanho <= 0) return NULL;
    LeitorBits* l = (LeitorBits*)malloc(sizeof(LeitorBits));
    if (!l) return NULL;
    l->buffer = buffer;
    l->tamanho = tamanho;
    l->pos_byte = 0;
    l->pos_bit = 0;
    return l;
}

/* Retorna 0/1 para bit lido, ou -1 se EOF. */
int leitorbits_ler_bit(LeitorBits* leitor) {
    if (!leitor) return -1;
    if (leitor->pos_byte >= leitor->tamanho) return -1;
    int shift = 7 - leitor->pos_bit;
    int bit = (leitor->buffer[leitor->pos_byte] >> shift) & 1;
    leitor->pos_bit++;
    if (leitor->pos_bit == 8) {
        leitor->pos_bit = 0;
        leitor->pos_byte++;
    }
    return bit;
}

int leitorbits_eof(LeitorBits* leitor) {
    if (!leitor) return 1;
    return leitor->pos_byte >= leitor->tamanho;
}

void leitorbits_destruir(LeitorBits* leitor) {
    free(leitor);
}

long arquivo_obter_tamanho(const char* nome_arquivo) {
    FILE* f = fopen(nome_arquivo, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long t = ftell(f);
    fclose(f);
    return t;
}

int arquivo_ler_tudo(const char* nome_arquivo, unsigned char* buffer, int maximo) {
    FILE* f = fopen(nome_arquivo, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);
    if (tamanho > maximo) { fclose(f); return -1; }
    size_t lidos = fread(buffer, 1, tamanho, f);
    fclose(f);
    return (int)lidos;
}

int arquivo_escrever_tudo(const char* nome_arquivo, const unsigned char* buffer, int tamanho) {
    FILE* f = fopen(nome_arquivo, "wb");
    if (!f) return -1;
    size_t escritos = fwrite(buffer, 1, tamanho, f);
    fclose(f);
    return (escritos == (size_t)tamanho) ? 0 : -1;
}

void imprimir_hex(const char* rotulo, const unsigned char* buffer, int tamanho) {
    printf("%s: ", rotulo);
    for (int i = 0; i < tamanho && i < 16; i++) {
        printf("%02X ", buffer[i]);
    }
    if (tamanho > 16) printf("...");
    printf("\n");
}

void imprimir_estatisticas(long original, long compactado) {
    if (original <= 0) return;
    double taxa = (1.0 - (double)compactado / original) * 100.0;
    printf("Original: %ld bytes\n", original);
    printf("Compactado: %ld bytes\n", compactado);
    printf("Taxa de compressão: %.2f%%\n", taxa);
}
