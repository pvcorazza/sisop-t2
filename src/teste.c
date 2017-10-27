//
// Created by aluno on 27/10/17.
//

#include <stdio.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/superblock.h"

int main () {

    struct t2fs_superbloco superbloco;

    printf("==== TESTE DO IDENTIFY2 ====\n\n");

    char nomes[200];
    identify2(nomes,200);
    puts(nomes);

    printf("==== LEITURA DO SUPERBLOCO ====\n\n");

    if(read_superblock(&superbloco)!= 0) {
        printf("Erro na leitura do superbloco \n\n");
    } else {
        printf("DEBUG: Superbloco foi lido corretamente \n\n");
        print_debug_superblock(superbloco);
    }

}