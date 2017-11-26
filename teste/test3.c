#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main() {

    // testa criação e remoção de arquivos regulares

    tree2(2,0);
    printf("\n");
    getchar();

    printf("CRIANDO ARQUIVOS...\n\n");
    create2("/file3.txt");
    create2("/dir3/file4.txt");
    create2("/file3.txt");                          // esse arquivo já existe nesse diretório. Precisa dar erro.
    create2("/dir2/file4.txt");
    create2("/dir2/dir5/dir8/file4.txt");
    create2("/dir2/dir5/dir8/file5.txt");
    create2("/dir2/dir5/dir8/dir9/dir10/file4.txt");
    create2("/dir4/file6.txt");                    // mesma coisa que o test2. /dir4 não existe e portanto file6.txt não será criado.
    create2("/file7.txt");
    create2("/dir1/file1.txt/file8.txt");          // file1.txt não é um diretório, e portanto, não pode criar file8.txt
    create2("/dir1/file8.txt");                    // file8.txt não existe, então não pode excluir
    create2("/dir3/dir7/file9.txt");
    create2("/dir3/dir6/file10.txt");               // dir6 foi excluído no último exemplo. Não pode criar file10.txt

    tree2(2,0);
    printf("\n");
    getchar();

    printf("REMOVENDO ARQUIVOS...\n\n");
    delete2("/file1.txt");
    delete2("/file20.txt");                       // esse arquivo não existe. Não deve deletar nada.
    delete2("/dir4/file1.txt");                   // como dir4 não existe, também não pode deletar file1.txt
    delete2("/dir2/dir5/dir8/dir9/dir10/file4.txt");
    delete2("/dir2/dir5/dir8/file5.txt");
    delete2("/dir1/file2.txt");
    delete2("/dir1");                             // não pode deletar dir1, pois não é arquivo regular

    tree2(2,0);
    printf("\n");
    getchar();

    return 0;
}
