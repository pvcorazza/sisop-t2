#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // testa cria��o e remo��o de arquivos diret�rios

    tree2(2,0);      /* A fun��o tree2 foi criada para debugging */
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
    mkdir2("/dir2");                        // N�o pode ser criado pois j� existe /dir2
    mkdir2("/dir10/dir11");                 // n�o funciona pois n�o h� /dir10 para colocar dir11. Isso est� certo?

    tree2(2,0);
    printf("\n");
    getchar();

    printf("REMOVENDO DIRETORIOS...\n\n");
    rmdir2("/dir2/dir4");
    rmdir2("/dir3/dir6");
    rmdir2("/dir3");                    // N�o remove dir3 pois este n�o est� vazio (ainda tem dir7). N�o seria melhor imprimir isso?
    rmdir2("/dir1");                    // mesmo problema do dir3, s� que com arquivos regulares dentro dele
    rmdir2("/dir1/file1.txt");          // n�o pode remover pois n�o � um diret�rio

    tree2(2,0);
    printf("\n");
    getchar();

    return 0;
}
