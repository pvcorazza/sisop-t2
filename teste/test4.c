#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // comentários partem do presuposto de que a sequência da realização dos testes é seguida em ordem crescente.
    // teste de aberturas e fechamentos de arquivos/diretórios

    tree2(2,0);
    printf("\n");
    getchar();

    printf("ABRINDO ARQUIVOS...\n");
    open2("/file3.txt");
    open2("/file1.txt");          // file1.txt não existe mais, portanto não pode abrir este arquivo
    open2("/dir2/dir5/dir8/file4.txt");
    open2("/dir3/dir7/file9.txt");
    open2("/dir1");              // precisa dar erro pois é um diretório, e não um arquivo

    printf("ABRINDO DIRETORIOS...\n");
    opendir2("/dir3/dir7");
    opendir2("/dir3");
    opendir2("/dir4");            // dir4 não existe. Não pode abrir este diretório
    opendir2("/dir2");
    opendir2("/dir2/dir5/dir8/dir9/dir10");
    opendir2("/dir2/file4.txt");     // não é um diretório, Não pode abrir este arquivo

    printf("\n");
    print_open_files();   /*  A função print_open_files foi criada para debugging */
    printf("\n");
    getchar();

    printf("FECHANDO DIRETORIOS...\n");
    closedir2(0);
    closedir2(2);
    closedir2(8);    // não há ninguém no handle 8

    printf("FECHANDO ARQUIVOS...\n");
    close2(1);
    close2(2);
    close2(9);      // não há ninguém no handle 9
    close2(100);    // handle 100 não está nem definido

    printf("\n");
    print_open_files();
    printf("\n");
    getchar();

    return 0;
}
