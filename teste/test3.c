#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // testa cria��o e remo��o de arquivos regulares

    tree2(2,0);
    printf("\n");
    getchar();

    printf("CRIANDO ARQUIVOS...\n\n");
    create2("/file3.txt");
    create2("/dir3/file4.txt");
    create2("/file3.txt");                          // esse arquivo j� existe nesse diret�rio. Precisa dar erro.
    create2("/dir2/file4.txt");
    create2("/dir2/dir5/dir8/file4.txt");
    create2("/dir2/dir5/dir8/file5.txt");
    create2("/dir2/dir5/dir8/dir9/dir10/file4.txt");
    create2("/dir4/file6.txt");                    // mesma coisa que o test2. /dir4 n�o existe e portanto file6.txt n�o ser� criado.
    create2("/file7.txt");
    create2("/dir1/file1.txt/file8.txt");          // file1.txt n�o � um diret�rio, e portanto, n�o pode criar file8.txt
    create2("/dir1/file8.txt");                    // file8.txt n�o existe, ent�o n�o pode excluir
    create2("/dir3/dir7/file9.txt");
    create2("/dir3/dir6/file10.txt");               // dir6 foi exclu�do no �ltimo exemplo. N�o pode criar file10.txt

    tree2(2,0);
    printf("\n");
    getchar();

    printf("REMOVENDO ARQUIVOS...\n\n");
    delete2("/file1.txt");
    delete2("/file20.txt");                       // esse arquivo n�o existe. N�o deve deletar nada.
    delete2("/dir4/file1.txt");                   // como dir4 n�o existe, tamb�m n�o pode deletar file1.txt
    delete2("/dir2/dir5/dir8/dir9/dir10/file4.txt");
    delete2("/dir2/dir5/dir8/file5.txt");
    delete2("/dir1/file2.txt");
    delete2("/dir1");                             // n�o pode deletar dir1, pois n�o � arquivo regular

    tree2(2,0);
    printf("\n");
    getchar();

    return 0;
}
