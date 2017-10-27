//
// Created by aluno on 27/10/17.
//

#include "t2fs.h"

#ifndef T2FS_SUPERBLOCK_H
#define T2FS_SUPERBLOCK_H

/* Chama as funções disponíveis para leitura de um setor lógico no disco
 * e armazena as informações necessárias no superbloco */

int read_superblock (struct t2fs_superbloco *superbloco);

/* Imprime informações sobre o superbloco */
void print_debug_superblock (struct t2fs_superbloco superbloco);

#endif //T2FS_SUPERBLOCK_H
