#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // coment�rios partem do presuposto de que a sequ�ncia da realiza��o dos testes � seguida em ordem crescente.
    // teste de aberturas e fechamentos de arquivos/diret�rios

    tree2(2,0);
    printf("\n");
    getchar();

    printf("ABRINDO ARQUIVOS...\n");
    open2("/file3.txt");
    open2("/file1.txt");          // file1.txt n�o existe mais, portanto n�o pode abrir este arquivo
    open2("/dir2/dir5/dir8/file4.txt");
    open2("/dir3/dir7/file9.txt");
    open2("/dir1");              // precisa dar erro pois � um diret�rio, e n�o um arquivo

    printf("ABRINDO DIRETORIOS...\n");
    opendir2("/dir3/dir7");
    opendir2("/dir3");
    opendir2("/dir4");            // dir4 n�o existe. N�o pode abrir este diret�rio
    opendir2("/dir2");
    opendir2("/dir2/dir5/dir8/dir9/dir10");
    opendir2("/dir2/file4.txt");     // n�o � um diret�rio, N�o pode abrir este arquivo

    printf("\n");
    print_open_files();   /*  A fun��o print_open_files foi criada para debugging */
    printf("\n");
    getchar();

    printf("FECHANDO DIRETORIOS...\n");
    closedir2(0);
    closedir2(2);
    closedir2(8);    // n�o h� ningu�m no handle 8

    printf("FECHANDO ARQUIVOS...\n");
    close2(1);
    close2(2);
    close2(9);      // n�o h� ningu�m no handle 9
    close2(100);    // handle 100 n�o est� nem definido

    printf("\n");
    print_open_files();
    printf("\n");
    getchar();

    return 0;
}
