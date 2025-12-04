#include "arvore_huffman.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Cria nó folha de Huffman. */
NoHuffman* nohuffman_criar_folha(unsigned char simbolo, int frequencia) {
    NoHuffman* n = (NoHuffman*)malloc(sizeof(NoHuffman));
    if (!n) return NULL;
    n->simbolo = simbolo;
    n->frequencia = frequencia;
    n->esquerda = n->direita = NULL;
    return n;
}

/* Cria nó interno que combina duas subárvores. */
NoHuffman* nohuffman_criar_interno(NoHuffman* esquerda, NoHuffman* direita) {
    NoHuffman* n = (NoHuffman*)malloc(sizeof(NoHuffman));
    if (!n) return NULL;
    n->simbolo = 0;
    n->frequencia = (esquerda ? esquerda->frequencia : 0) + (direita ? direita->frequencia : 0);
    n->esquerda = esquerda;
    n->direita = direita;
    return n;
}

/* Libera memória (pós-ordem). */
void arvorehuffman_destruir(NoHuffman* raiz) {
    if (!raiz) return;
    arvorehuffman_destruir(raiz->esquerda);
    arvorehuffman_destruir(raiz->direita);
    free(raiz);
}

/* Constrói árvore usando algoritmo guloso simples (seleção O(n^2)). */
NoHuffman* arvorehuffman_construir(const int* frequencias) {
    NoHuffman* lista[256];
    int n = 0;
    for (int i = 0; i < 256; ++i) {
        if (frequencias[i] > 0) {
            lista[n++] = nohuffman_criar_folha((unsigned char)i, frequencias[i]);
        }
    }
    if (n == 0) return NULL;
    while (n > 1) {
        int i_min1 = -1, i_min2 = -1;
        for (int i = 0; i < n; ++i) {
            if (i_min1 == -1 || lista[i]->frequencia < lista[i_min1]->frequencia) {
                i_min2 = i_min1; i_min1 = i;
            } else if (i_min2 == -1 || lista[i]->frequencia < lista[i_min2]->frequencia) {
                i_min2 = i;
            }
        }
        /* garantir ordem */
        if (i_min2 < i_min1) { int t = i_min1; i_min1 = i_min2; i_min2 = t; }
        NoHuffman* a = lista[i_min1];
        NoHuffman* b = lista[i_min2];
        NoHuffman* comb = nohuffman_criar_interno(a, b);
        lista[i_min1] = comb;
        /* remove i_min2 */
        for (int k = i_min2; k < n - 1; ++k) lista[k] = lista[k+1];
        n--;
    }
    return lista[0];
}

/* Gera tabela de códigos (inicializa e chama recursiva). */
void arvorehuffman_gerar_tabela(NoHuffman* raiz, TabelaHuffman* tabela) {
    if (!tabela) return;
    for (int i = 0; i < 256; ++i) {
        tabela->codigo[i] = NULL;
        tabela->tamanho[i] = 0;
    }
    if (!raiz) return;
    char codigo_temp[512];
    arvorehuffman_gerar_tabela_rec(raiz, codigo_temp, 0, tabela);
}

/* Recursiva: preenche tabela ao alcançar folhas. */
void arvorehuffman_gerar_tabela_rec(NoHuffman* no, char* codigo, int profundidade, TabelaHuffman* tabela) {
    if (!no) return;
    if (nohuffman_eh_folha(no)) {
        codigo[profundidade] = '\0';
        int len = profundidade;
        tabela->codigo[no->simbolo] = (char*)malloc(len + 1);
        if (tabela->codigo[no->simbolo]) memcpy(tabela->codigo[no->simbolo], codigo, len + 1);
        tabela->tamanho[no->simbolo] = len;
        return;
    }
    /* esquerda -> '0' */
    codigo[profundidade] = '0';
    arvorehuffman_gerar_tabela_rec(no->esquerda, codigo, profundidade + 1, tabela);
    /* direita -> '1' */
    codigo[profundidade] = '1';
    arvorehuffman_gerar_tabela_rec(no->direita, codigo, profundidade + 1, tabela);
}

/* Helper recursiva de serialização. */
static int arvorehuffman_serializar_rec(NoHuffman* n, unsigned char* buffer, int tam_buffer, int* pos) {
    if (!n) return 0;
    if (nohuffman_eh_folha(n)) {
        if (*pos + 2 > tam_buffer) return -1;
        buffer[(*pos)++] = 0x01;
        buffer[(*pos)++] = n->simbolo;
        return 0;
    } else {
        if (*pos + 1 > tam_buffer) return -1;
        buffer[(*pos)++] = 0x00;
        if (arvorehuffman_serializar_rec(n->esquerda, buffer, tam_buffer, pos) < 0) return -1;
        if (arvorehuffman_serializar_rec(n->direita, buffer, tam_buffer, pos) < 0) return -1;
        return 0;
    }
}

/* Serializa árvore em pré-ordem. Retorna bytes escritos ou -1. */
int arvorehuffman_serializar(NoHuffman* raiz, unsigned char* buffer, int tam_buffer) {
    if (!buffer || tam_buffer <= 0) return -1;
    int pos = 0;
    if (!raiz) return 0;
    if (arvorehuffman_serializar_rec(raiz, buffer, tam_buffer, &pos) < 0) return -1;
    return pos;
}

/* Helper recursiva de desserialização. */
static NoHuffman* arvorehuffman_desserializar_rec(const unsigned char* buffer, int tam_buffer, int* pos) {
    if (*pos >= tam_buffer) return NULL;
    unsigned char t = buffer[(*pos)++];
    if (t == 0x01) {
        if (*pos >= tam_buffer) return NULL;
        unsigned char simb = buffer[(*pos)++];
        return nohuffman_criar_folha(simb, 0);
    } else if (t == 0x00) {
        NoHuffman* e = arvorehuffman_desserializar_rec(buffer, tam_buffer, pos);
        NoHuffman* d = arvorehuffman_desserializar_rec(buffer, tam_buffer, pos);
        return nohuffman_criar_interno(e, d);
    } else {
        return NULL;
    }
}

/* Desserializa árvore; atualiza 'consumidos' se fornecido. */
NoHuffman* arvorehuffman_desserializar(const unsigned char* buffer, int tam_buffer, int* consumidos) {
    if (!buffer || tam_buffer <= 0) {
        if (consumidos) *consumidos = 0;
        return NULL;
    }
    int pos = 0;
    NoHuffman* raiz = arvorehuffman_desserializar_rec(buffer, tam_buffer, &pos);
    if (consumidos) *consumidos = pos;
    return raiz;
}

int nohuffman_eh_folha(NoHuffman* no) {
    return no && (no->esquerda == NULL && no->direita == NULL);
}

/* Impressão para debug (pré-ordem com indent). */
static void arvorehuffman_imprimir_rec(NoHuffman* no, int nivel) {
    if (!no) return;
    for (int i = 0; i < nivel; ++i) printf("  ");
    if (nohuffman_eh_folha(no)) {
        unsigned char c = no->simbolo;
        if (c >= 32 && c <= 126)
            printf("Leaf '%c' (%u) freq=%d\n", c, c, no->frequencia);
        else
            printf("Leaf (0x%02X) freq=%d\n", c, no->frequencia);
    } else {
        printf("Node freq=%d\n", no->frequencia);
    }
    arvorehuffman_imprimir_rec(no->esquerda, nivel + 1);
    arvorehuffman_imprimir_rec(no->direita, nivel + 1);
}

void arvorehuffman_imprimir(NoHuffman* raiz) {
    arvorehuffman_imprimir_rec(raiz, 0);
}
