#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // comentários partem do presuposto de que a sequência da realização dos testes é seguida em ordem crescente.
    // testa à exaustão criações no diretório, que só pode ter, no máximo, 14 arquivos (pois ocupa só um cluster!)

    printf("CRIANDO DIRETORIOS NO /DIR1...\n");
    char *arquivos[] = {"/dir1/dir100",
                        "/dir1/dir101",
                        "/dir1/dir102",
                        "/dir1/dir103",
                        "/dir1/dir104",
                        "/dir1/dir105",
                        "/dir1/dir106",
                        "/dir1/dir107",
                        "/dir1/dir108",
                        "/dir1/dir109",
                        "/dir1/dir110",
                        "/dir1/dir111",
                        "/dir1/dir112",
                        "/dir1/dir113",
                        "/dir1/dir114",
                        "/dir1/dir115",
                        "/dir1/dir116",
                        "/dir1/dir117",
                        "/dir1/dir118",
                        "/dir1/dir119",
                        "/dir1/dir120",
                        "/dir1/dir121",
                        "/dir1/dir122",
                        "/dir1/dir123",
                        "/dir1/dir124",
                        "/dir1/dir125",
                        "\0"};

    int i=0, j;
    while(*arquivos[i] != '\0') {
        printf("Criando diretorio: %s\n", arquivos[i]);
        j = mkdir2(arquivos[i++]);
        printf("Retorno: %d\n", j);
        printf("\n");
        tree2(2,0);
        printf("\n");
        getchar();
    }

    return 0;
}
