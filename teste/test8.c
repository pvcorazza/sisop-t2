#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // comentários partem do presuposto de que a sequência da realização dos testes é seguida em ordem crescente.
    // testa truncate dos arquivos escritos no test7.c

    printf("ABRINDO ARQUIVOS PARA TESTE...\n");
    open2("/file3.txt");
    open2("/dir2/file4.txt");
    open2("/dir8/b.txt");
    open2("/file7.txt");
    open2("/dir8/filefile.txt");

    getchar();
    printf("\n");
    print_open_files();
    printf("\n");
    getchar();

    seek2(0,1792);    // tamanho é 1792, não deve perder nada
    seek2(1,1000);    // novo tamanho deve ser 1304
    seek2(2,0);       // deve apagar tudo do arquivo b.txt que foi completado com todos aqueles 'kkkkkk...'
    seek2(3,600);
    seek2(4,200);     // como o arquivo 4 tem tamanho zero, CP deve ficar no lugar e pra truncar, não vai truncar nada

    getchar();
    printf("\n");
    print_open_files();
    printf("\n");
    getchar();

    printf("TRUNCANDO ARQUIVOS...\n");
    truncate(0);
    truncate(1);
    truncate(2);
    truncate(3);
    truncate(4);

    printf("LENDO ARQUIVOS...");
    int returnValue;
    int size = 10000;
    char buffer[size];

    returnValue = read2(0, buffer, size);
    printf("Retorno: %d\nBuffer: %s\n", returnValue, buffer);    // imprime para cada arquivo aberto o valor de retorno e o conteúdo colocado em buffer
    getchar();
    returnValue = read2(1, buffer, size);
    printf("Retorno: %d\nBuffer: %s\n", returnValue, buffer);
    getchar();
    returnValue = read2(2, buffer, size);
    printf("Retorno: %d\nBuffer: %s\n", returnValue, buffer);
    getchar();
    returnValue = read2(3, buffer, size);
    printf("Retorno: %d\nBuffer: %s\n", returnValue, buffer);
    getchar();
    returnValue = read2(4, buffer, size);
    printf("Retorno: %d\nBuffer: %s\n", returnValue, buffer);
    getchar();

    return 0;
}
