#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // comentários partem do presuposto de que a sequência da realização dos testes é seguida em ordem crescente.
    // testa à exaustação abertura e fechamento de arquivos/diretórios, pra ver o que vai acontecer quando ultrapassar o limite de arquivos abertos

    printf("CRIANDO NOVOS ARQUIVOS E DIRETORIOS...\n");
    mkdir2("/dir2/dir5");   // já existe. Não pode abrir dir5 em dir2
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
                        "/dir5/file1.txt",         // /dir5/file1.txt não existe. Não pode abrir este.
                        "/dir2/dir5/dir8/file4.txt",
                        "/dir3/dir7",              // dir7 é um diretório. Não pode abrir como arquivo.
                        "/dir3/file4.txt",
                        "/dir8/b.txt",
                        "/dir8/b.txt",             // este arquivo já está aberto. Pode abrir novamente (porque foi aberto novamente) ?
                        "/file3.txt",              // neste é pra ultrapassar o limite de arquivos abertos e não inserir mais nada. (podia informar que não pode ser aberto por falta de espaço)
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
                          "/dir100",                           // esse diretório não existe. Não pode abrir
                          "/dir1/file8.txt",                   // file8.txt é um arquivo regular, não um diretório. Não pode abrir
                          "/dir2/dir5/dir8/dir9/dir10/dir11",
                          "/dir2/dir5/dir8/dir9/dir10/a",
                          "/dir3/dir7",                         // já está aberto. Pode abrir novamente (porque foi aberto novamente) ?
                          "/dir8",
                          "/dir3/dir7/dir18",
                          "/dir2/dir5/dir8/dir9",
                          "/dir2/dir5/dir8",
                          "/dir1",                             // neste é pra ultrapassar o limite de diretórios abertos!
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
