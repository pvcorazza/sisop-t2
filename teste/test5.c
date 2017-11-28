#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // coment�rios partem do presuposto de que a sequ�ncia da realiza��o dos testes � seguida em ordem crescente.
    // testa � exausta��o abertura e fechamento de arquivos/diret�rios, pra ver o que vai acontecer quando ultrapassar o limite de arquivos abertos

    printf("CRIANDO NOVOS ARQUIVOS E DIRETORIOS...\n");
    mkdir2("/dir2/dir5");   // j� existe. N�o pode abrir dir5 em dir2
    mkdir2("/dir8");
    mkdir2("/dir3/dir7/dir18");
    create2("/dir8/filefile.txt");
    create2("/dir3/sisop.txt");
    mkdir2("/dir2/dir5/dir8/dir9/dir10/dir11");
    mkdir2("/dir2/dir5/dir8/dir9/dir10/a");
    create2("/dir8/b.txt");

    tree2(2,0);
    printf("\n");
    printf("\n");
    print_open_files();
    printf("\n");
    getchar();

    printf("ABRINDO ARQUIVOS...\n");
    char *arquivos[] = {"/dir8/filefile.txt",
                        "/file2.txt",
                        "/dir2/file4.txt",
                        "/dir5/file1.txt",         // /dir5/file1.txt n�o existe. N�o pode abrir este.
                        "/dir2/dir5/dir8/file4.txt",
                        "/dir3/dir7",              // dir7 � um diret�rio. N�o pode abrir como arquivo.
                        "/dir3/file4.txt",
                        "/dir8/b.txt",
                        "/dir8/b.txt",             // este arquivo j� est� aberto. Pode abrir novamente (porque foi aberto novamente) ?
                        "/file3.txt",              // neste � pra ultrapassar o limite de arquivos abertos e n�o inserir mais nada. (podia informar que n�o pode ser aberto por falta de espa�o)
                        "/file7.txt",
                        "/dir3/dir7/file9.txt",
                        "/dir1/file1.txt",
                        "/dir3/sisop.txt",
                        "\0"};

    int i=0;
    while(*arquivos[i] != '\0') {
        printf("Abrindo arquivo: %s\n", arquivos[i]);
        open2(arquivos[i++]);
        printf("\n");
        print_open_files();
        printf("\n");
        getchar();
    }

    printf("ABRINDO DIRETORIOS...\n");
    char *diretorios[] = {"/dir2",
                          "/dir2/dir5",
                          "/dir3/dir7",
                          "/dir3",
                          "/dir100",                           // esse diret�rio n�o existe. N�o pode abrir
                          "/dir1/file8.txt",                   // file8.txt � um arquivo regular, n�o um diret�rio. N�o pode abrir
                          "/dir2/dir5/dir8/dir9/dir10/dir11",
                          "/dir2/dir5/dir8/dir9/dir10/a",
                          "/dir3/dir7",                         // j� est� aberto. Pode abrir novamente (porque foi aberto novamente) ?
                          "/dir8",
                          "/dir3/dir7/dir18",
                          "/dir2/dir5/dir8/dir9",
                          "/dir2/dir5/dir8",
                          "/dir1",                             // neste � pra ultrapassar o limite de diret�rios abertos!
                          "\0"};

    i=0;
    while(*diretorios[i] != '\0') {
        printf("Abrindo diretorio: %s\n", diretorios[i]);
        opendir2(diretorios[i++]);
        printf("\n");
        print_open_files();
        printf("\n");
        getchar();
    }

    return 0;
}
