/*
 * Universidade Federal do Rio Grande do Sul (UFRGS)
 * Sistemas Operacionais I - Trabalho 2
 *
 * Giovani Tirello, Marcelo Wille, Paulo Victor Corazza
 */

#include "../include/apidisk.h"
#include "../include/t2fs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Array para armazenar os diretórios abertos */
struct dir_aberto diretorios_abertos[MAX_ABERTOS];
struct arq_aberto arquivos_abertos[MAX_ABERTOS];

/* Variável que indica se é a primeira vez que tenta executar a API (se sim, o superbloco ainda não foi lido) */
int first_time = 1;
char current_path[8192] = "/";

int inicializa() {

    if (read_superblock() == 0) {

        TOTAL_SETORES_FAT = SUPERBLOCO.DataSectorStart - SUPERBLOCO.pFATSectorStart;
        NUM_CLUSTERS = ((SUPERBLOCO.NofSectors - SUPERBLOCO.DataSectorStart) / SUPERBLOCO.SectorsPerCluster);

        first_time = 0;
        inicializa_fat();
        return 0;

    }

    exit(1);
}

void print_superbloco_info() {

    if (first_time) {
        inicializa();
    }

    printf("Dados do superbloco:\n");
    printf("Identificacao do sistema de arquivos: %c%c%c%c\n", SUPERBLOCO.id[0], SUPERBLOCO.id[1], SUPERBLOCO.id[2],
           SUPERBLOCO.id[3]);
    printf("Versao do Sistema de Arquivos: %X\n", SUPERBLOCO.version);
    printf("Numero de setores logicos do superbloco: %d\n", SUPERBLOCO.SuperBlockSize);
    printf("Tamanho total da particao (em bytes): %d\n", SUPERBLOCO.DiskSize);
    printf("Numero de setores logicos na particao: %d\n", SUPERBLOCO.NofSectors);
    printf("Numero de setores logicos por cluster: %d\n", SUPERBLOCO.SectorsPerCluster);
    printf("Primeiro setor logico da FAT: %d\n", SUPERBLOCO.pFATSectorStart);
    printf("Cluster onde inicia o arquivo correspondente ao diretorio raiz: %d\n", SUPERBLOCO.RootDirCluster);
    printf("Primeiro setor logico da area de blocos de dados ou cluster: %d\n\n", SUPERBLOCO.DataSectorStart);
    printf("OUTROS DADOS SOBRE O SISTEMA DE ARQUIVOS:\n\n");
    printf("Numero total de clusters: %d\n", NUM_CLUSTERS);
    printf("Numero de setores lógicos ocupados pela FAT: %d\n", TOTAL_SETORES_FAT);

}

static int read_superblock() {

    BYTE buffer[SECTOR_SIZE];

    if (read_sector(0, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Leitura do superbloco não foi feita com sucesso!\n\n");
        return -1;
    } else {
        memcpy(&SUPERBLOCO, buffer, 32);
        return 0;
    }
}

static int inicializa_fat() {
    int i, j = 0;
    for (i = SUPERBLOCO.pFATSectorStart; i <= TOTAL_SETORES_FAT + 1; i++) {
        if (read_sector((unsigned int) i, (unsigned char *) &FAT[j]) != 0) {
            printf("Não foi possível fazer a leitura da FAT. \n");
            return -1;
        } else {
            j = j + 64;
        }
    }
    return 0;
}


void imprime_conteudo_fat() {
    if (first_time) {
        inicializa();
    }
    int i;
    for (i = 0; i < NUM_CLUSTERS; i++) {
        if (FAT[i] != 0) {
            printf("Cluster %d: %X\n", i, FAT[i]);
        }

    }
}

void le_diretorio(int cluster) {
    if (first_time) {
        inicializa();
    }

    struct t2fs_record raiz;

    int i;

    for (i = 0; i < 4; i++) {

        unsigned char buffer[SECTOR_SIZE];
        read_sector(SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

        int j = 0;
        for (j = 0; j < SECTOR_SIZE; j = j + 64) {
            memcpy(&raiz, &buffer[j], 64);
            if (raiz.TypeVal != 0) {
                printf("Tipo da entrada: Inválido=0, Arquivo = 1, Diretorio = 2: %d", raiz.TypeVal);
                printf("\nNome do arquivo: %s", raiz.name);
                printf("\nTamanho do arquivo: %d", raiz.bytesFileSize);
                printf("\nPrimeiro cluster de dados: %d\n\n", raiz.firstCluster);
            }
        }
    }
}

int conta_entradas_diretorio(int cluster) {
    if (first_time) {
        inicializa();
    }

    struct t2fs_record record;

    int i;
    int entradas = 0;

    for (i = 0; i < 4; i++) {

        unsigned char buffer[SECTOR_SIZE];
        read_sector(SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

        int j = 0;
        for (j = 0; j < SECTOR_SIZE; j = j + 64) {
            memcpy(&record, &buffer[j], 64);
            if (record.TypeVal != 0) {
                entradas++;
            }
        }
    }
    return entradas;
}

int insere_entrada(int cluster, struct t2fs_record entrada, int posicao) {
    if (first_time) {
        inicializa();
    }

    int primeiro_setor_cluster = SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster;

    int setor = primeiro_setor_cluster + (posicao / 4);

    int pos_inserir = posicao % 4;

    unsigned char buffer[SECTOR_SIZE];

    if (read_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("Não foi possível fazer a leitura da entrada. \n");
        return -1;
    }

    memcpy(&buffer[pos_inserir * 64], &entrada, 64);


    if (write_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("Não foi possível fazer a escrita da entrada. \n");
        return -1;
    }
}


int busca_entrada_livre_dir(int cluster) {
    struct t2fs_record raiz;

    int i;
    int entrada = 0;

    for (i = 0; i < 4; i++) {

        unsigned char buffer[SECTOR_SIZE];
        read_sector(SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

        int j = 0;
        for (j = 0; j < SECTOR_SIZE; j = j + 64) {
            memcpy(&raiz, &buffer[j], 64);
            if (raiz.TypeVal == 0) {
                return entrada;
            }
            entrada++;
        }
    }
    return -1;
}

int busca_posicao_entrada(char *name, int cluster) {
    struct t2fs_record record;

    int i;
    int entrada = 0;

    for (i = 0; i < 4; i++) {
        unsigned char buffer[SECTOR_SIZE];
        read_sector(SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

        int j = 0;
        for (j = 0; j < SECTOR_SIZE; j = j + 64) {
            memcpy(&record, &buffer[j], 64);
            if (strcmp(name, record.name) == 0) {
                return entrada;
            }
            entrada++;
        }
    }
    return -1;
}


void caminho_para_array(char *string, char *array[]) {

    if (first_time) {
        inicializa();
    }
    int i = 0;

    array[i] = strtok(string, "/");

    while (array[i] != NULL) {
        array[++i] = strtok(NULL, "/");
    }
}

int divide_caminho(char *pathname, char *inicio, char *final) {

    char temp_inicio[strlen(pathname + 1)];
    strcpy(temp_inicio, pathname);

    char *temp_final = strrchr(temp_inicio, '/');
    if (temp_final == NULL) {
        printf("Não pode dividir caminho");
        return -1;
    }

    strcpy(final, temp_final + 1);
    temp_inicio[temp_final - temp_inicio] = '\0';
    strcpy(inicio, temp_inicio);

    return 0;
}

struct t2fs_record compara_nomes(int cluster, char *pathname) {

    if (first_time) {
        inicializa();
    }
    char string[strlen(pathname)];
    strcpy(string, pathname);

    char *array[NUM_CLUSTERS]; //C-1 clusters possíveis para o diretório

    caminho_para_array(string, array);

    int i = 0;

    int k = 0;
    int encontrou = 0;
    int temp_cluster = cluster;

    struct t2fs_record vazio = {0};
    struct t2fs_record record = {0};
    struct t2fs_record record_retorno = {0};

    while (array[k] != NULL) {
        for (i = 0; i < 4; i++) {

            unsigned char buffer[SECTOR_SIZE];
            read_sector(SUPERBLOCO.DataSectorStart + temp_cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

            int j = 0;
            for (j = 0; j < SECTOR_SIZE; j = j + 64) {
                memcpy(&record, &buffer[j], 64);
                if (!strcmp(record.name, array[k])) {
                    encontrou = 1;
                    temp_cluster = record.firstCluster;
                    record_retorno = record;
                }
            }
        }
        if (encontrou == 0) {
            return vazio;
        }
        encontrou = 0;
        k++;
    }
    return record_retorno;

}

DIR2 busca_pos_array_dir() {
    if (first_time) {
        inicializa();
    }
    int i = 0;
    while (i < MAX_ABERTOS) {
        if (diretorios_abertos[i].aberto != 1) {
            return i;
        }
        i++;
    }
    return -1;
}

FILE2 busca_pos_array_arq() {
    if (first_time) {
        inicializa();
    }
    int i = 0;
    while (i < MAX_ABERTOS) {
        if (arquivos_abertos[i].aberto != 1) {
            return i;
        }
        i++;
    }
    return -1;
}

int busca_pos_livre_FAT() {

    DWORD buffer[64];
    int i = SUPERBLOCO.pFATSectorStart, j = 0, k = 0;

    while (i <= TOTAL_SETORES_FAT + 1) {
        if (read_sector((unsigned int) i, (unsigned char *) &buffer) != 0) {
            printf("Não foi possível fazer a leitura da FAT. \n");
            return -1;
        } else {
            j = 0;
            while (j < 64) {
                if (buffer[j] == 0) {
                    return k;
                }
                k++;
                j++;
            }
        }
        i++;
    }

    return -1;

}

int insere_entrada_FAT(int posicao, DWORD entrada) {

    int setor = posicao / SECTOR_SIZE + SUPERBLOCO.pFATSectorStart;

    DWORD buffer[64];

    if (read_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("Não foi possível fazer a leitura da FAT. \n");
        return -1;
    }

    while (buffer[posicao] >= 0x00000002 && buffer[posicao] <= 0xFFFFFFFD) {
        printf("ENTROU NO WHILE, posicao: %d, buffer: %X\n", posicao, buffer[posicao]);
        int proxima = buffer[posicao];
        buffer[posicao] = entrada;
        if (write_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
            printf("Não foi possível fazer a escrita da FAT. \n");
            return -1;
        }
        posicao = proxima;
    }

    buffer[posicao] = entrada;
    if (write_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("Não foi possível fazer a escrita da FAT. \n");
        return -1;
    }

    return 0;
}

DWORD encontra_proximo_setor(int cluster) {
    int setor = (cluster / SECTOR_SIZE) + SUPERBLOCO.pFATSectorStart;

    DWORD buffer[64];

    if (read_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("Não foi possível fazer a leitura da FAT. \n");
        return 0xFFFFFFFF;
    }

    if (buffer[cluster] >= 0x00000002 && buffer[cluster] <= 0xFFFFFFFD) {
        return buffer[cluster];
    }

    return 0xFFFFFFFF;

}

int le_bytes_arquivo(int size, int cluster, int file_size, char *buffer) {

    int primeiro_setor = cluster * 4 + SUPERBLOCO.DataSectorStart;
    char aux[SECTOR_SIZE];

    int tamanho = file_size + (SECTOR_SIZE * 4) + 1;

    char temp2[tamanho];

    temp2[0] = '\0';

    int i = 0;

    for (i = 0; i < 4; i++) {
        if (read_sector((unsigned int) primeiro_setor + i, (unsigned char *) &aux) == 0) {
            strcat(temp2, aux);
        } else {
            return -1;
        }
    }

    DWORD next = encontra_proximo_setor(cluster);

    while (next != 0xFFFFFFFF) {
        for (i = 0; i < 4; i++) {
            if (read_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + i, (unsigned char *) &aux) == 0) {
                strcat(temp2, aux);
            } else {
                return -1;
            }
        }
        next = encontra_proximo_setor(next);
    }

    memcpy(buffer, temp2, size);

    return 0;
}


/* Informa a identificação dos desenvolvedores do t2fs. */
int identify2(char *name, int size) {

    if (first_time) {
        inicializa();
    }

    char names[] = "Giovani Tirello (252741)\nMarcelo Wille (228991)\nPaulo Corazza (192172)\n";

    if (size < sizeof(names) / sizeof(char)) {
        return -1;        //Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro
    } else {
        strcpy(name, names);
        return 0;         //Caso contrário copia a string para o endereço de memória indicada por name.
    }

}


FILE2 create2(char *filename) {

    if (first_time) {
        inicializa();
    }

    char inicio[strlen(filename)];
    char final[strlen(filename)];

    if (divide_caminho(filename, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    record = compara_nomes(SUPERBLOCO.RootDirCluster, filename);

    if (record.TypeVal != NULL) {
        printf("ERRO: O arquivo ja existe.\n");
        return -1;
    }

    diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio);

    if (diretorio_pai.TypeVal == TYPEVAL_DIRETORIO) { //Pode criar arquivo

        int posicao_FAT = busca_pos_livre_FAT();

        if (posicao_FAT >= 0) {

            int posicao_dir = busca_entrada_livre_dir(diretorio_pai.firstCluster);

            if (posicao_dir >= 0) {

                if (insere_entrada_FAT(posicao_FAT, 0xFFFFFFFF) < 0) {
                    return -1;
                }

                struct t2fs_record novo;

                strcpy(novo.name, final);
                novo.firstCluster = (DWORD) posicao_FAT;
                novo.TypeVal = TYPEVAL_REGULAR;
                novo.bytesFileSize = 0;

                if (insere_entrada(diretorio_pai.firstCluster, novo, posicao_dir) < 0) {
                    return -1;
                }

                FILE2 handle = busca_pos_array_arq();

                if (handle >= 0 && handle <= MAX_ABERTOS) {
                    arquivos_abertos[handle].arquivo = record;
                    arquivos_abertos[handle].aberto = 1;
                    arquivos_abertos[handle].current_pointer = 0; //Conferir se é isso mesmo.
                    return handle;
                }
            }
        }
    }
    return -1;
}


int delete2(char *filename) {
    if (first_time) {
        inicializa();
    }

    char inicio[strlen(filename)];
    char final[strlen(filename)];

    if (divide_caminho(filename, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    record = compara_nomes(SUPERBLOCO.RootDirCluster, filename);

    if (record.TypeVal == TYPEVAL_REGULAR) {

        struct t2fs_record novo = {0};

        diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio);

        int posicao_dir = busca_posicao_entrada(final, diretorio_pai.firstCluster);

        if (insere_entrada(diretorio_pai.firstCluster, novo, posicao_dir) < 0) {
            return -1;
        }

        if (insere_entrada_FAT(record.firstCluster, 0x00000000) < 0) {
            return -1;
        }

        //Verificar se é necessário zerar o cluster do arquivo também, ou somente a entrada na FAT.

        return 0;

    }
    return -2;
}

FILE2 open2(char *filename) {
    if (first_time) {
        inicializa();
    }

    FILE2 handle = busca_pos_array_arq();
    struct t2fs_record record;

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        record = compara_nomes(SUPERBLOCO.RootDirCluster, filename);
        if (record.name != NULL && record.TypeVal == TYPEVAL_REGULAR) {
            arquivos_abertos[handle].arquivo = record;
            arquivos_abertos[handle].aberto = 1;
            arquivos_abertos[handle].current_pointer = 0; //Conferir se é isso mesmo.
            return handle;
        }
    }

    return -1;
}

int close2(FILE2 handle) {
    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (arquivos_abertos[handle].aberto == 1) {
            arquivos_abertos[handle].aberto = 0;
            return 0;
        }
    }

    return -1;
}

int read2(FILE2 handle, char *buffer, int size) {

    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (arquivos_abertos[handle].aberto == 1) {
            if (size > arquivos_abertos[handle].arquivo.bytesFileSize) {
                int retorno = le_bytes_arquivo(arquivos_abertos[handle].arquivo.bytesFileSize,
                                               arquivos_abertos[handle].arquivo.firstCluster,
                                               arquivos_abertos[handle].arquivo.bytesFileSize, buffer);
                if (retorno >= 0) {
                    // puts(buffer);
                    arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].arquivo.bytesFileSize + 1;
                    return arquivos_abertos[handle].arquivo.bytesFileSize;
                }
            } else {
                int retorno = le_bytes_arquivo(size, arquivos_abertos[handle].arquivo.firstCluster,
                                               arquivos_abertos[handle].arquivo.bytesFileSize, buffer);
                if (retorno >= 0) {
                    arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].current_pointer + size;
                    // puts(buffer);
                    return size;


                }
            }
        }
    }
    return -1;
}


int write2(FILE2 handle, char *buffer, int size) {
    return -1;
}

int truncate2(FILE2 handle) {
    return -1;
}

int seek2(FILE2 handle, unsigned int offset) {
    return -1;
}


int mkdir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    char inicio[strlen(pathname)];
    char final[strlen(pathname)];

    if (divide_caminho(pathname, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname);

    if (record.TypeVal != NULL) {
        printf("ERRO: O diretorio ja existe.\n");
        return -1;
    }

    diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio);

    if (diretorio_pai.TypeVal == TYPEVAL_DIRETORIO) { //Pode criar diretório

        int posicao_FAT = busca_pos_livre_FAT();

        if (posicao_FAT >= 0) {

            int posicao_dir = busca_entrada_livre_dir(diretorio_pai.firstCluster);

            if (posicao_dir >= 0) {

                if (insere_entrada_FAT(posicao_FAT, 0xFFFFFFFF) < 0) {
                    return -1;
                }

                struct t2fs_record novo;

                strcpy(novo.name, final);
                novo.firstCluster = (DWORD) posicao_FAT;
                novo.TypeVal = TYPEVAL_DIRETORIO;
                novo.bytesFileSize = 1024;

                if (insere_entrada(diretorio_pai.firstCluster, novo, posicao_dir) < 0) {
                    return -1;
                }

                strcpy(novo.name, ".");
                if (insere_entrada(novo.firstCluster, novo, 0) < 0) {
                    return -1;
                }

                strcpy(diretorio_pai.name, "..");
                if (insere_entrada(novo.firstCluster, diretorio_pai, 1) < 0) {
                    return -1;
                }

                return 0;
            }

        }
    }
    return -1;
}

int rmdir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    char inicio[strlen(pathname)];
    char final[strlen(pathname)];

    if (divide_caminho(pathname, inicio, final) < 0) {
        return -1;
    }

    if ((strcmp(final, ".") == 0) || (strcmp(final, ".") == 0)) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname);

    if (record.TypeVal == TYPEVAL_DIRETORIO) {

        int entradas = conta_entradas_diretorio(record.firstCluster);


        if (entradas == 2) {

            struct t2fs_record novo = {0};

            if (insere_entrada(record.firstCluster, novo, 0) < 0) {
                return -1;

            }

            if (insere_entrada(record.firstCluster, novo, 1) < 0) {
                return -1;
            }

            diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio);

            int posicao_dir = busca_posicao_entrada(final, diretorio_pai.firstCluster);

            if (insere_entrada(diretorio_pai.firstCluster, novo, posicao_dir) < 0) {
                return -1;
            }

            if (insere_entrada_FAT(record.firstCluster, 0x00000000) < 0) {
                return -1;
            }

            return 0;
        }
    }
    return -2;
}


int chdir2(char *pathname) {

    struct t2fs_record current;

    current = compara_nomes(SUPERBLOCO.RootDirCluster, pathname);

    if (current.TypeVal == TYPEVAL_DIRETORIO) {
        strcpy(current_path, pathname);
        return 0;
    }

    return -1;
}

int getcwd2(char *pathname, int size) {

    if (first_time) {
        inicializa();
    }

    if (size < strlen(current_path)) {
        return -1;        //Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro
    } else {
        strcpy(pathname, current_path);
        return 0;         //Caso contrário copia a string para o endereço de memória indicada por name.
    }
}

DIR2 opendir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    DIR2 handle = busca_pos_array_dir();
    struct t2fs_record record;

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname);
        if (record.name != NULL && record.TypeVal == TYPEVAL_DIRETORIO) {
            diretorios_abertos[handle].diretorio = record;
            diretorios_abertos[handle].aberto = 1;
            diretorios_abertos[handle].current_entry = 0; //Conferir se é isso mesmo.
            return handle;
        }
    }

    return -1;

}

#define    END_OF_DIR    1

int readdir2(DIR2 handle, DIRENT2 *dentry) {
    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (diretorios_abertos[handle].diretorio.TypeVal == TYPEVAL_DIRETORIO) {

            struct t2fs_record record;
            int current_entry = diretorios_abertos[handle].current_entry;
            int i;

            /* Faz uma busca pelo cluster de diretório pela entrada de número "current_entry",
             * caso encontre, copia os dados para a estrutura dentry, incrementa a "current_entry
             * do diretório e retorna indicando sucesso */
            for (i = 0; i < 4; i++) {
                unsigned char buffer[SECTOR_SIZE];
                read_sector(SUPERBLOCO.DataSectorStart +
                            diretorios_abertos[handle].diretorio.firstCluster * SUPERBLOCO.SectorsPerCluster + i,
                            &buffer[0]);

                int j = 0;
                for (j = 0; j < SECTOR_SIZE; j = j + 64) {
                    memcpy(&record, &buffer[j], 64);
                    if (record.TypeVal == 1 || record.TypeVal == 2) {
                        if (current_entry == 0) {

                            dentry->fileType = record.TypeVal;
                            dentry->fileSize = record.bytesFileSize;
                            strcpy(dentry->name, record.name);

                            diretorios_abertos[handle].current_entry++;

                            return 0;
                        }
                        current_entry--;
                    }
                }
            }

            /* Caso ja tenha chegado na última entrada válida do diretório
             * retorna erro */
            current_entry++;
            if (current_entry > 0) {
                return -END_OF_DIR;
            }
        }
    }
    return -2;
}

int closedir2(DIR2 handle) {

    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (diretorios_abertos[handle].aberto == 1) {
            diretorios_abertos[handle].aberto = 0;
            return 0;
        }
    }

    return -1;

}


