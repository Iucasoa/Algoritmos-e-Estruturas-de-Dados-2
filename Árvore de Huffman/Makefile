# ---------------------------------------------------------
# Makefile para o Projeto de Compressão Huffman em C
# Compila: arvore_huffman.c, utilitarios.c, compressor.c,
#          descompressor.c e main.c
# Gera o executável: huffman
# ---------------------------------------------------------

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
OBJ = arvore_huffman.o utilitarios.o compressor.o descompressor.o main.o
TARGET = huffman

# Regra padrão (compilar tudo)
all: $(TARGET)

# Linkagem final
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Compilação individual de cada .c
arvore_huffman.o: arvore_huffman.c arvore_huffman.h
	$(CC) $(CFLAGS) -c arvore_huffman.c

utilitarios.o: utilitarios.c utilitarios.h
	$(CC) $(CFLAGS) -c utilitarios.c

compressor.o: compressor.c arvore_huffman.h utilitarios.h
	$(CC) $(CFLAGS) -c compressor.c

descompressor.o: descompressor.c arvore_huffman.h utilitarios.h
	$(CC) $(CFLAGS) -c descompressor.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# ---------------------------------------------------------
# Comandos extras
# ---------------------------------------------------------

# Remove binários e objetos
clean:
	rm -f $(OBJ) $(TARGET)

# Executa um teste padrão (modifique o arquivo se desejar)
# Ex: make run FILE=animacao.tex
run: $(TARGET)
	./$(TARGET) comprimir $(FILE) $(FILE).huff

# Testa compressão e descompressão automaticamente
# make test FILE=animacao.tex
test: $(TARGET)
	./$(TARGET) comprimir $(FILE) teste.huff
	./$(TARGET) descomprimir teste.huff restaurado.txt
	cmp --silent $(FILE) restaurado.txt && \
		echo "OK: Arquivo restaurado é idêntico!" || \
		echo "ERRO: Arquivos diferentes!"
	rm -f teste.huff restaurado.txt

# Recompilar tudo do zero
rebuild: clean all

# ---------------------------------------------------------
# Fim do Makefile
# ---------------------------------------------------------
