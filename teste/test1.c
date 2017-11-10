#include "../include/t2fs.h"

#include <stdio.h>
#include <string.h>

int main () {

    struct t2fs_superbloco SUPERBLOCO;

    printf("==== TESTE DO IDENTIFY2 ====\n\n");

    char nomes[200];
    identify2(nomes,200);
    puts(nomes);

    printf("==== LEITURA DO SUPERBLOCO ====\n\n");

    if(read_superblock(&SUPERBLOCO) == 0)
        print_debug_superblock(SUPERBLOCO);

}
