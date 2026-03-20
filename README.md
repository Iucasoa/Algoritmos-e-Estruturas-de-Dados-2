<h1 align="center">📦 Huffman File Compressor</h1>

<p align="center">
  <img src="https://img.shields.io/badge/Status-Finalizado-success?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white" />
  <img src="https://img.shields.io/badge/CS-Data_Structures-orange?style=for-the-badge" />
</p>

<p align="center">
  <b>Implementação do Algoritmo de Huffman para compressão e descompressão de arquivos sem perda de dados (Lossless).</b><br>
  <i>Projeto prático desenvolvido para a disciplina de Estrutura de Dados II - UFERSA.</i>
</p>

---

### 📝 Sobre o Projeto
Este programa utiliza a codificação de Huffman para reduzir o tamanho de arquivos de texto e binários. O algoritmo analisa a frequência de cada caractere e gera códigos binários de tamanho variável: caracteres mais frequentes recebem códigos menores, resultando na compactação do arquivo final.

---

### 🛠️ Especificações Técnicas

| Componente | Implementação |
| :--- | :--- |
| **Estrutura Base** | Árvore Binária de Caracteres |
| **Priorização** | Frequência de ocorrência (Min-Heap / Lista Ordenada) |
| **Codificação** | Mapeamento por Tabela de Dicionário (Hash Map simples) |
| **Manipulação** | Operadores Bitwise (`<<`, `>>`, `&`, `|`) para escrita em bits |

---

### ⚙️ Como funciona?

[Image of Huffman coding tree construction steps]

1. **Cálculo de Frequência:** O arquivo é lido para contar a ocorrência de cada byte.
2. **Construção da Árvore:** Os nós são agrupados em uma árvore binária onde as folhas são os caracteres.
3. **Geração do Dicionário:** É percorrida a árvore para gerar os novos códigos (Esquerda = 0, Direita = 1).
4. **Compressão:** O arquivo original é reescrito bit a bit usando o novo mapeamento.
5. **Cabeçalho:** A árvore é salva junto ao arquivo comprimido para permitir a reconstrução posterior.

---

### 📊 Performance Esperada
* **Arquivos de Texto:** Redução média de **40% a 60%**.
* **Arquivos Aleatórios:** Menor taxa de compressão (depende da entropia dos dados).

---

### 📁 Estrutura do Código
* `huffman.c`: Lógica principal de construção da árvore e codificação.
* `bit_manager.h`: Funções para ler e escrever bits individuais em arquivos.
* `main.c`: Interface via linha de comando para compressão/descompressão.

---

### 🚀 Como usar
```bash
# Para comprimir:
./huffman -c arquivo.txt arquivo.huff

# Para descomprimir:
./huffman -d arquivo.huff arquivo_original.txt
