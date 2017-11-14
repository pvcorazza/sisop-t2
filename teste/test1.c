#include "../include/t2fs.h"

#include <stdio.h>

int main () {

    printf("==== TESTE DO IDENTIFY2 ====\n\n");
    char nomes[200];
    identify2(nomes,200);
    puts(nomes);

    printf("==== DADOS DO SUPERBLOCO ====\n\n");
    print_superbloco_info();

    printf("\n======= FAT =======\n\n");
    inicializa_fat(SUPERBLOCO.DataSectorStart - SUPERBLOCO.pFATSectorStart);
    imprime_conteudo_fat(FAT, ((SUPERBLOCO.NofSectors - SUPERBLOCO.DataSectorStart) / SUPERBLOCO.SectorsPerCluster));



    create2("/fileOne");

    return 0;

}
