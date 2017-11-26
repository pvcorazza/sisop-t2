#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // testa criação e remoção de arquivos diretórios

    tree2(2,0);      /* A função tree2 foi criada para debugging */
    printf("\n");
    getchar();

    printf("CRIANDO DIRETORIOS...\n\n");
    mkdir2("/dir2");
    mkdir2("/dir3");
    mkdir2("/dir2/dir4");
    mkdir2("/dir2/dir5");
    mkdir2("/dir3/dir6");
    mkdir2("/dir3/dir7");
    mkdir2("/dir2/dir5/dir8");
    mkdir2("/dir2/dir5/dir8/dir9");
    mkdir2("/dir2/dir5/dir8/dir9/dir10");
    mkdir2("/dir2");                        // Não pode ser criado pois já existe /dir2
    mkdir2("/dir10/dir11");                 // não funciona pois não há /dir10 para colocar dir11. Isso está certo?

    tree2(2,0);
    printf("\n");
    getchar();

    printf("REMOVENDO DIRETORIOS...\n\n");
    rmdir2("/dir2/dir4");
    rmdir2("/dir3/dir6");
    rmdir2("/dir3");                    // Não remove dir3 pois este não está vazio (ainda tem dir7). Não seria melhor imprimir isso?
    rmdir2("/dir1");                    // mesmo problema do dir3, só que com arquivos regulares dentro dele
    rmdir2("/dir1/file1.txt");          // não pode remover pois não é um diretório

    tree2(2,0);
    printf("\n");
    getchar();

    return 0;
}
