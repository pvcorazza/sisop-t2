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
            printf("ERRO: Nao foi possivel fazer a leitura da FAT 1. \n");
            return -1;
        } else {
            j = j + 64;
        }
    }
    return 0;
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
    printf("Numero de setores logicos ocupados pela FAT: %d\n", TOTAL_SETORES_FAT);

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
            if (raiz.TypeVal == TYPEVAL_DIRETORIO || raiz.TypeVal == TYPEVAL_REGULAR) {
                printf("Tipo da entrada: Invalido=0, Arquivo = 1, Diretorio = 2:  %d", raiz.TypeVal);
                printf("\nNome do arquivo: %s", raiz.name);
                printf("\nTamanho do arquivo: %d", raiz.bytesFileSize);
                printf("\nPrimeiro cluster de dados: %d\n\n", raiz.firstCluster);
            }
        }
    }
}

/* Função auxiliar à função tree2 definida abaixo, para imprimir determinado número de espaços em branco na tela */
void printspace(int offset) {
    int i;
    for(i=0; i<offset; i++)
        printf(" ");
}

/* Função de debugging para imprimir a árvore do sistema de arquivos. Offset é o deslocamento das linhas para impressão, devendo ser zero inialmente. */
void tree2(int cluster, int offset) {

    if (first_time) {
        inicializa();
    }

    struct t2fs_record record;
    int i,j;

    if(cluster == SUPERBLOCO.RootDirCluster)
        printf("\n/\n");

    for(i=0; i<4; i++) {
        unsigned char buffer[SECTOR_SIZE];
        read_sector(SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);
        for (j = 0; j < SECTOR_SIZE; j = j + 64) {    // j começa de 128 para não pegar '.' e '..'
            memcpy(&record, &buffer[j], 64);
            if (record.TypeVal != 0 && (i!=0 || j>=128)) {   // (p->q)  é o mesmo que (~p v q). Se 'i' for zero, só passa do if se j>=128, evitando assim '.' '..'
                printspace(offset);
                printf("|\n");
                printspace(offset);
                printf("|");
                if(record.TypeVal == 2) {
                    printf("-----%s\n", record.name);
                    tree2(record.firstCluster, offset+8);
                }
                else {
                    printf("-----%s\n", record.name);
                }
            }
        }
    }
}

/* Função de debugging para imprimir os arquivos (regulares e diretórios) abertos */
void print_open_files() {

    int handle;

    printf("DIRETORIOS ABERTOS:\n");
    for(handle = 0; handle<MAX_ABERTOS; handle++) {
        if(diretorios_abertos[handle].aberto == 1) {
            printf("HANDLE = %d\n", handle);
            printf("Nome: %s\n", diretorios_abertos[handle].diretorio.name);
            printf("Primeiro Cluster: %d\n", diretorios_abertos[handle].diretorio.firstCluster);
            printf("Tamanho: %d\n", diretorios_abertos[handle].diretorio.bytesFileSize);
            printf("Current_Entry: %d\n", diretorios_abertos[handle].current_entry);
            printf("---------------------------------------\n");
        }
    }
    printf("\nARQUIVOS REGULARES ABERTOS:\n");
    for(handle = 0; handle<MAX_ABERTOS; handle++) {
        if(arquivos_abertos[handle].aberto == 1) {
            printf("HANDLE = %d\n", handle);
            printf("Nome: %s\n", arquivos_abertos[handle].arquivo.name);
            printf("Primeiro Cluster: %d\n", arquivos_abertos[handle].arquivo.firstCluster);
            printf("Tamanho: %d\n", arquivos_abertos[handle].arquivo.bytesFileSize);
            printf("Current_Pointer: %d\n", arquivos_abertos[handle].current_pointer);
            printf("---------------------------------------\n");
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
            if (record.TypeVal == TYPEVAL_DIRETORIO || record.TypeVal == TYPEVAL_REGULAR) {
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
        printf("ERRO: Nao foi possivel fazer a leitura da entrada. \n");
        return -1;
    }

    memcpy(&buffer[pos_inserir * 64], &entrada, 64);


    if (write_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Nao foi possivel fazer a escrita da entrada. \n");
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
            if (raiz.TypeVal != TYPEVAL_REGULAR && raiz.TypeVal != TYPEVAL_DIRETORIO) {
                return entrada;
            }
            entrada++;
        }
    }
    return -1;
}

int busca_posicao_entrada(char *name, int cluster) {
    struct t2fs_record record;

//    printf("NOME %s, CLUSTER: %d\n", name, cluster);

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

int verifica_absoluto(char *path) {

    if (path[0] == '/') {
        return 0;
    }

    return -1;

}

int insere_caminho_raiz(char *novo, char *path) {
    novo[0] = '/';
    novo[1] = '.';
    novo[2] = '\0';


    if (verifica_absoluto(path) != 0) {
        printf("ERRO. Caminho nao eh absoluto.");
        return -1;
    }

    if (strcmp(path, "/") != 0) {
        strcat(novo, path);
    }
    return 0;
}

void formata_caminho(char *pathname, char *string) {
//    printf("PATHNAME DENTRO DO FORMATA CAMINHO: %s\n", pathname);

    if (verifica_absoluto(pathname) == 0) {
        insere_caminho_raiz(string, pathname);
    } else {
        strcpy(string, current_path);
        if (strcmp(current_path, "/") == 0) {
            strcat(string, pathname);
        } else {
            strcat(string, "/");
            strcat(string, pathname);
        }
    }
}

int divide_caminho(char *pathname, char *inicio, char *final) {

    /* Formata caminho para correta divisão entre caminho e nome do arquivo */

    char caminho_formatado[strlen(pathname) + strlen(current_path) + 2];
    formata_caminho(pathname, caminho_formatado);

    char temp_inicio[strlen(caminho_formatado + 1)];
    strcpy(temp_inicio, caminho_formatado);

    char *temp_final = strrchr(temp_inicio, '/');
    if (temp_final == NULL) {
        printf("ERRO: Nao pode dividir caminho");
        return -1;
    }

    strcpy(final, temp_final + 1);
    temp_inicio[temp_final - temp_inicio] = '\0';
    strcpy(inicio, temp_inicio);

    return 0;
}

struct t2fs_record compara_nomes(int cluster, char *pathname, int typeval) {

    if (first_time) {
        inicializa();
    }

    char caminho_formatado[strlen(pathname) + strlen(current_path) + 2];

    formata_caminho(pathname, caminho_formatado);

    char string[strlen(caminho_formatado)];

    if (strcmp(caminho_formatado, "/") == 0) {
        strcpy(string, ".");
    } else {
        strcpy(string, caminho_formatado);
    }

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
                if (!strcmp(record.name, array[k]) && (array[k + 1] != NULL || typeval == record.TypeVal)) {
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
            printf("ERRO: Nao foi possivel fazer a leitura da FAT 2. \n");
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
        printf("ERRO: Nao foi possivel fazer a leitura da FAT 3. \n");
        return -1;
    }

    while (buffer[posicao] >= 0x00000002 && buffer[posicao] <= 0xFFFFFFFD) {
        printf("ENTROU NO WHILE, posicao: %d, buffer: %X\n", posicao, buffer[posicao]);
        int proxima = buffer[posicao];
        buffer[posicao] = entrada;
        if (write_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
            printf("ERRO: Nao foi possivel fazer a escrita da FAT. \n");
            return -1;
        }
        posicao = proxima;
    }

    buffer[posicao] = entrada;
    if (write_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Nao foi possivel fazer a escrita da FAT. \n");
        return -1;
    }

    return 0;
}

DWORD encontra_proximo_setor(int cluster) {
    int setor = (cluster / SECTOR_SIZE) + SUPERBLOCO.pFATSectorStart;

    DWORD buffer[64];

    if (read_sector((unsigned int) setor, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Nao foi possivel fazer a leitura da FAT 4. \n");
        return 0xFFFFFFFF;
    }

    if (buffer[cluster] >= 0x00000002 && buffer[cluster] <= 0xFFFFFFFD) {
        return buffer[cluster];
    }

    return 0xFFFFFFFF;

}

int le_bytes_arquivo(int size, FILE2 handle, char *buffer) {

    printf("ENTROU AQUI");

    /* Se o tamanho informado é maior que a diferença entre o current pointer e o tamanho, atualiza o tamanho a ser
     * lido para somente essa diferença. */
    if (size > arquivos_abertos[handle].arquivo.bytesFileSize - arquivos_abertos[handle].current_pointer) {
        size = arquivos_abertos[handle].arquivo.bytesFileSize - arquivos_abertos[handle].current_pointer;
        printf("SIZE: %d\n\n", size);
    }

    int tamanho = arquivos_abertos[handle].arquivo.bytesFileSize + (SECTOR_SIZE * 4) + SECTOR_SIZE;
    char total_bytes_lidos[tamanho];
    total_bytes_lidos[0] = '\0';

    int offset_cluster = (arquivos_abertos[handle].current_pointer / SECTOR_SIZE) / 4;
//    printf("OFFSET CLUSTER= %d", offset_cluster);
    int offset_cluster_original = offset_cluster;
    int i = 0;

    DWORD next = arquivos_abertos[handle].arquivo.firstCluster;

    while (offset_cluster > 0) {
        next = encontra_proximo_setor(next);
        offset_cluster--;
    }

    int offset_setor = (arquivos_abertos[handle].current_pointer / SECTOR_SIZE) - 4 * offset_cluster_original;
    int diferenca =
            arquivos_abertos[handle].current_pointer - (arquivos_abertos[handle].current_pointer / 256) * SECTOR_SIZE;

    char aux[SECTOR_SIZE];

    for (i = 0; i < 4 - offset_setor; i++) {
        if (read_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + i + offset_setor,
                        (unsigned char *) &aux) == 0) {
            strcat(total_bytes_lidos, aux);
        } else {
            return -1;
        }
    }

    next = encontra_proximo_setor(next);

    while (next != 0xFFFFFFFF) {
        for (i = 0; i < 4; i++) {
            if (read_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + i, (unsigned char *) &aux) == 0) {
                strcat(total_bytes_lidos, aux);
            } else {
                return -1;
            }
        }
        next = encontra_proximo_setor(next);
    }

    memcpy(buffer, &total_bytes_lidos[diferenca], (size_t) size);

    return size;
}

int escreve_bytes_dentro_arquivo(int size, FILE2 handle, char *buffer) {

    int tamanho_a_escrever = size;

    /* Se o tamanho informado é maior que a diferença entre o current pointer e o tamanho, atualiza o tamanho a ser
    * escrito para somente essa diferença. */
    if (size > arquivos_abertos[handle].arquivo.bytesFileSize - arquivos_abertos[handle].current_pointer) {
        tamanho_a_escrever = arquivos_abertos[handle].arquivo.bytesFileSize - arquivos_abertos[handle].current_pointer;
    }

    int tamanho_a_escrever_original = tamanho_a_escrever;

    printf("TAMANHO A ESCREVER ORIGINAL %d\n", tamanho_a_escrever_original);

    int tamanho = arquivos_abertos[handle].arquivo.bytesFileSize + (SECTOR_SIZE * 4) + SECTOR_SIZE;
    char total_bytes_lidos[tamanho];
    total_bytes_lidos[0] = '\0';

    int offset_cluster = (arquivos_abertos[handle].current_pointer / SECTOR_SIZE) / 4;
//    printf("OFFSET CLUSTER= %d", offset_cluster);
    int offset_cluster_original = offset_cluster;
    int i = 0;

    DWORD next = arquivos_abertos[handle].arquivo.firstCluster;

    while (offset_cluster > 0) {
        next = encontra_proximo_setor(next);
        offset_cluster--;
    }

    int offset_setor = (arquivos_abertos[handle].current_pointer / SECTOR_SIZE) - 4 * offset_cluster_original;
    int diferenca =
            arquivos_abertos[handle].current_pointer - (arquivos_abertos[handle].current_pointer / 256) * SECTOR_SIZE;


    char read_aux[SECTOR_SIZE];
    char aux[SECTOR_SIZE];


    if (read_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + offset_setor, (unsigned char *) &read_aux) ==
        0) {

        if ((SECTOR_SIZE - diferenca) < tamanho_a_escrever) {
            memcpy(&read_aux[diferenca], &buffer[0], (size_t) SECTOR_SIZE - diferenca);
            printf("TAMANHO A ESCREVERL: %d\n", tamanho_a_escrever);
            printf("DIFERENca: %d\n", diferenca);
            int teste = SECTOR_SIZE - diferenca;
            tamanho_a_escrever = tamanho_a_escrever - teste;
        } else {
            printf("READ AUX ORIGINAL: %s\n", read_aux);
            printf("BUFFER ORIGINAL: %s\n", buffer);
            printf("TAMANHO A ESCREVER: %d\n", tamanho_a_escrever);

            memcpy(&read_aux[diferenca], &buffer[0], (size_t) tamanho_a_escrever);
            tamanho_a_escrever = 0;
        }

        printf("READ AUX: %s\n", read_aux);
        write_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + offset_setor, (unsigned char *) &read_aux);
        printf("PRIMEIRO OFFSET SETOR %d\n", offset_setor);
        if (tamanho_a_escrever == 0) {
            printf("RETORNO NO PRIMEIRO SETOR");
            return size - tamanho_a_escrever_original;
        }

        printf("TAMANHO A ESCREVER ANTES DO PRIMEIRO WHILE: %d\n", tamanho_a_escrever);

        while (++offset_setor < 4) {
            printf("OFFSET SETOR %d\n", offset_setor);
            if (read_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + offset_setor,
                            (unsigned char *) &aux) == 0) {
                printf("O QUE FALTA ESCREVER: %d\n", tamanho_a_escrever_original - tamanho_a_escrever);
                if (SECTOR_SIZE < tamanho_a_escrever) {


                    memcpy(&aux[0], &buffer[tamanho_a_escrever_original - tamanho_a_escrever], (size_t) SECTOR_SIZE);
                    tamanho_a_escrever = tamanho_a_escrever - SECTOR_SIZE;
                } else {
                    memcpy(&aux[0], &buffer[tamanho_a_escrever_original - tamanho_a_escrever],
                           (size_t) tamanho_a_escrever);
                    tamanho_a_escrever = 0;
                }
                printf("AUX 2: %s\n", aux);

                write_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart + offset_setor,
                             (unsigned char *) &aux);
                if (tamanho_a_escrever == 0) {
                    printf("RETORNO NO PRIMEIRO CLUSTER");
                    return size - tamanho_a_escrever_original;
                }
            } else {
                return -1;
            }
        }


    } else {
        return -1;
    }

    next = encontra_proximo_setor(next);

    while (next != 0xFFFFFFFF && tamanho_a_escrever != 0) {
        for (i = 0; i < 4; i++) {
            if (read_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart, (unsigned char *) &aux) == 0) {

                printf("O NEXT É %d\n", next);
                printf("O AUX É %s\n", aux);

                getchar();

                if (SECTOR_SIZE < tamanho_a_escrever) {
                    memcpy(&aux[0], &buffer[tamanho_a_escrever_original - tamanho_a_escrever], (size_t) SECTOR_SIZE);
                    tamanho_a_escrever = tamanho_a_escrever - SECTOR_SIZE;
                } else {
                    memcpy(&aux[0], &buffer[tamanho_a_escrever_original - tamanho_a_escrever],
                           (size_t) tamanho_a_escrever);
                    tamanho_a_escrever = 0;
                }


                write_sector((unsigned int) next * 4 + SUPERBLOCO.DataSectorStart, (unsigned char *) &aux);
                if (tamanho_a_escrever == 0) {
                    printf("RETORNO NO CLUSTER %d", next);
                    return size - tamanho_a_escrever_original;
                }

            } else {
                return -1;
            }
        }
        next = encontra_proximo_setor(next);
    }

    return size - tamanho_a_escrever_original;


}

int escreve_bytes_final_arquivo(int size, FILE2 handle, char *buffer) {

    /* Cópia de buffer e inicialização com 0 */

    char copia_buffer[size + SECTOR_SIZE];
    memset(copia_buffer, 0, (size_t) (size + SECTOR_SIZE));
    memcpy(&copia_buffer, buffer, (size_t) size);

    copia_buffer[size] = '\0';

//    printf("COPIA BUFFER: %s\n", copia_buffer);

//    getchar();

    char aux[SECTOR_SIZE] = {0};

    /* Busca por ultimo cluster alocado do arquivo */

    DWORD next = arquivos_abertos[handle].arquivo.firstCluster;
    int anterior = arquivos_abertos[handle].arquivo.firstCluster;

    while (next != 0xFFFFFFFF) {
        anterior = next;
        next = encontra_proximo_setor(next);
    }

    int deslocamento_cluster = (arquivos_abertos[handle].arquivo.bytesFileSize / SECTOR_SIZE) % 4;

    if (read_sector((unsigned int) anterior * 4 + deslocamento_cluster + SUPERBLOCO.DataSectorStart,
                    (unsigned char *) &aux) != 0) {


        return -1;
    }

//    printf("DESLOCAMENTO CLUSTER: %d\n", deslocamento_cluster);

    int num_bytes_escritos = arquivos_abertos[handle].arquivo.bytesFileSize -
                             (arquivos_abertos[handle].arquivo.bytesFileSize / SECTOR_SIZE) *
                             SECTOR_SIZE; /* Bytes escritos no último bloco */
    int num_bytes_livres = SECTOR_SIZE - num_bytes_escritos; /* Bytes livres no último bloco */

//    printf("Num bytes escritos: %d\n", num_bytes_escritos);
//    printf("Num bytes livres: %d\n", num_bytes_livres);

    // getchar();

    /* Se é possivel escrever em somente um setor */


    if (size <= num_bytes_livres) {
        memcpy(&aux[num_bytes_escritos], copia_buffer, (size_t) size);
        if (write_sector((unsigned int) anterior * 4 + deslocamento_cluster + SUPERBLOCO.DataSectorStart,
                         (unsigned char *) &aux) != 0) {
            printf("ERRO: Nao foi possivel fazer a escrita da entrada. \n");
            return -1;
        }
    } else {

        int qtd_copiada = 0; /* Variável a ser incrementada conforme são escritos os bytes */

        /* Escreve no espaço restante no ultimo bloco do arquivo */

        memcpy(&aux[num_bytes_escritos], copia_buffer, (size_t) num_bytes_livres);

        if (write_sector((unsigned int) anterior * 4 + deslocamento_cluster + SUPERBLOCO.DataSectorStart,
                         (unsigned char *) &aux) != 0) {
            printf("ERRO: Nao foi possivel fazer a escrita da entrada. \n");
            return -1;
        }
        qtd_copiada = qtd_copiada + num_bytes_livres;

        /* Preenche o restante do último cluster já alocado do arquivo */

        int j;
        int restante = (SECTOR_SIZE * 4 - num_bytes_escritos) / SECTOR_SIZE;

        for (j = deslocamento_cluster + 1; j < 4 && qtd_copiada < size; j++) {
            memcpy(&aux, &copia_buffer[qtd_copiada], (size_t) SECTOR_SIZE);
            printf("AUX: %s", aux);

//            getchar();

            if (write_sector((unsigned int) anterior * 4 + j + SUPERBLOCO.DataSectorStart, (unsigned char *) &aux) !=
                0) {
                printf("ERRO: Nao foi possivel fazer a escrita da entrada. \n");
                return -1;
            }

            qtd_copiada = qtd_copiada + SECTOR_SIZE;
        }

        /* Faz uma busca, escrevendo nos clusters livres até o final da entrada */

        int proximo_setor = busca_pos_livre_FAT();

        while (qtd_copiada < size) {

            insere_entrada_FAT(anterior, (DWORD) proximo_setor);

            insere_entrada_FAT(proximo_setor, 0xFFFFFFFF);

            anterior = proximo_setor;

            proximo_setor = busca_pos_livre_FAT();

//            inicializa();
//            imprime_conteudo_fat();
//
//            getchar();

            int i = 0;

            while (qtd_copiada < size && i < 4) {

                char novo[SECTOR_SIZE] = {0};
                memcpy(&novo, &copia_buffer[qtd_copiada], (size_t) SECTOR_SIZE);

                if (write_sector((unsigned int) (anterior * 4) + i + SUPERBLOCO.DataSectorStart,
                                 (unsigned char *) &novo) != 0) {
                    printf("ERRO: Nao foi possivel fazer a escrita da entrada. \n");
                    return -1;
                }

                printf("NOVO: %s", aux);

//                getchar();
                qtd_copiada = qtd_copiada + SECTOR_SIZE;
                i++;
            }
        }
        //insere_entrada_FAT(anterior, 0xFFFFFFFF);
    }

    /* Atualiza o tamanho do arquivo na sua entrada correspondente no diretório pai */

    arquivos_abertos[handle].arquivo.bytesFileSize = arquivos_abertos[handle].arquivo.bytesFileSize + size;


//    printf("NOME ANTES DE ENTRAR : ");
//    puts(arquivos_abertos[handle].arquivo.name);


    int posicao_dir = busca_posicao_entrada(arquivos_abertos[handle].arquivo.name,
                                            arquivos_abertos[handle].diretorio_pai.firstCluster);

    if (posicao_dir < 0) {
        printf("Posicao dir %d\n", posicao_dir);
        return -1;
    }

    if (insere_entrada(arquivos_abertos[handle].diretorio_pai.firstCluster, arquivos_abertos[handle].arquivo,
                       posicao_dir) < 0) {
        printf("PROBLEMA NO INSERE ENTRADA\n");

        return -1;
    }

    return size;
}


int identify2(char *name, int size) {

    if (first_time) {
        inicializa();
    }

    char names[] = "Giovani Tirello (252741)\nMarcelo Wille (228991)\nPaulo Corazza (192172)\n";

    /* Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro,
     * caso contrário copia os nomes para o endereço de memória indicada por 'name' */
    if (size < sizeof(names) / sizeof(char)) {
        return -1;
    } else {
        strcpy(name, names);
        return 0;
    }
}

FILE2 create2(char *filename) {

    if (first_time) {
        inicializa();
    }

    /* Divide entre caminho e nome do arquivo */
    char inicio[strlen(filename) + strlen(current_path) + 2];
    char final[strlen(filename) + strlen(current_path) + 2];

    if (divide_caminho(filename, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    /* Verifica se arquivo já existe */
    record = compara_nomes(SUPERBLOCO.RootDirCluster, filename, TYPEVAL_REGULAR);

    if (record.TypeVal == TYPEVAL_REGULAR) {
        printf("ERRO: O arquivo ja existe.\n");
        return -1;
    }

    diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio, TYPEVAL_DIRETORIO);

    /* Se existe um diretório pai pode criar o arquivo */
    if (diretorio_pai.TypeVal == TYPEVAL_DIRETORIO) {

        /* Busca posição para alocar o arquivo na FAT */
        int posicao_FAT = busca_pos_livre_FAT();

        if (posicao_FAT >= 0) {

            /* Caso encontre posição na FAT, busca posição livre no diretório pai */
            int posicao_dir = busca_entrada_livre_dir(diretorio_pai.firstCluster);

            /* Ao encontrar posição livre no diretório pai, insere a entrada correspondente ao arquivo na FAT,
             * no diretório pai e no array de arquivos abertos retornando o handle do arquivo criado. */
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
                    arquivos_abertos[handle].arquivo = novo;
                    printf("Nome no create %s\n", arquivos_abertos[handle].arquivo.name);
                    arquivos_abertos[handle].aberto = 1;
                    arquivos_abertos[handle].current_pointer = 0; //Conferir se é isso mesmo.
                    arquivos_abertos[handle].diretorio_pai = diretorio_pai;
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

    /* Divide entre caminho e nome do arquivo */
    char inicio[strlen(filename) + strlen(current_path) + 2];
    char final[strlen(filename) + strlen(current_path) + 2];

    if (divide_caminho(filename, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    /* Verifica se arquivo existe */
    record = compara_nomes(SUPERBLOCO.RootDirCluster, filename, TYPEVAL_REGULAR);

    /* Se arquivo existe, remove sua entrada do diretório pai e zera sua entrada na FAT */
    if (record.TypeVal == TYPEVAL_REGULAR) {

        struct t2fs_record novo = {0};

        diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio, TYPEVAL_DIRETORIO);

        int posicao_dir = busca_posicao_entrada(final, diretorio_pai.firstCluster);

        if (insere_entrada(diretorio_pai.firstCluster, novo, posicao_dir) < 0) {
            return -1;
        }

        if (insere_entrada_FAT(record.firstCluster, 0x00000000) < 0) {
            return -1;
        }

        return 0;

    }
    return -2;
}


FILE2 open2(char *filename) {

    if (first_time) {
        inicializa();
    }

    struct t2fs_record pai;

    /* Busca por um handle disponível no array de arquivos abertos */
    FILE2 handle = busca_pos_array_arq();
    struct t2fs_record record;

    /* Se handle está no intervalo possível, busca e verifica a existência do arquivo informado,
     * busca pelo seu diretório pai, o insere no array de arquivos abertos, marcando como aberto,
     * seta seu current pointer para 0 e devolve o handle do arquivo criado. */
    if (handle >= 0 && handle <= MAX_ABERTOS) {
        record = compara_nomes(SUPERBLOCO.RootDirCluster, filename, TYPEVAL_REGULAR);

        char inicio[strlen(filename) + strlen(current_path) + 2];
        char final[strlen(filename) + strlen(current_path) + 2];

        if (divide_caminho(filename, inicio, final) < 0) {
            return -1;
        }

        pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio, TYPEVAL_DIRETORIO);

        if (record.name != NULL && record.TypeVal == TYPEVAL_REGULAR) {
            arquivos_abertos[handle].arquivo = record;
            arquivos_abertos[handle].aberto = 1;
            arquivos_abertos[handle].current_pointer = 0;
            arquivos_abertos[handle].diretorio_pai = pai;
            return handle;
        }
    }

    return -1;
}

int close2(FILE2 handle) {
    if (first_time) {
        inicializa();
    }

    /* Se o handle informado está dentro do intervalo possível para arquivos abertos
    * e o arquivo com o handle informado está aberto, marca arquivo como fechado */
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

    /* Se o handle informado está dentro do intervalo possível para arquivos abertos,
     * o arquivo com o handle informado está aberto e seu current pointer não ultrapassou
     * o final do arquivo, realiza a leitura de "size" bytes, atualiza o current pointer
     * e retorna o numero de bytes lidos */
    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (arquivos_abertos[handle].aberto == 1) {
            if (arquivos_abertos[handle].current_pointer <= arquivos_abertos[handle].arquivo.bytesFileSize) {
                int retorno = le_bytes_arquivo(size, handle, buffer);
                if (retorno >= 0) {
                    arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].current_pointer + retorno + 1;
                    return retorno;
                }
                return -2;
            }
            return -3;
        }
        return -1;
    }
    return -1;
}


int write2(FILE2 handle, char *buffer, int size) {

    if (first_time) {
        inicializa();
    }

    /* Se o handle informado está dentro do intervalo possível para arquivos abertos e
     * o arquivo com o handle informado está aberto, realiza a escrita de "size" bytes
     * disponíveis em 'buffer', atualiza o current pointer do arquivo para o final e
     * e retorna o numero de bytes escritos */
    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (arquivos_abertos[handle].aberto == 1) {


            if (arquivos_abertos[handle].current_pointer > arquivos_abertos[handle].arquivo.bytesFileSize) {
                int retorno = escreve_bytes_final_arquivo(size, handle, buffer);
                if (retorno >= 0) {
                    arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].arquivo.bytesFileSize + 1;
                    return retorno;
                }
            } else {
                int restante = escreve_bytes_dentro_arquivo(size, handle, buffer);
                printf("RESTANTE APOS ESCREVE BYTES DENTRO ARQUIVO: %d\n ", restante);
                if (restante > 0) {
                    char aux[size];
                    memcpy(&aux, &buffer[size - restante + 1], (size_t) restante);
                    printf("AUX: %s", aux);


                    int retorno = escreve_bytes_final_arquivo(restante, handle, aux);
                    if (retorno >= 0) {
                        arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].arquivo.bytesFileSize + 1;
                        return retorno;
                    }
                } else {
                    arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].current_pointer + size + 1;
                    return size;

                }
            }



            return -3;
        }
        return -1;
    }
    return -1;
}


int truncate2(FILE2 handle) {
    if (first_time) {
        inicializa();
    }


    /* Se o handle informado está dentro do intervalo possível para arquivos abertos e
     * o arquivo com o handle informado está aberto */
    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (arquivos_abertos[handle].aberto == 1) {

            int novo_tamanho_arquivo = arquivos_abertos[handle].current_pointer;

            int novo_num_clusters = 1 + (novo_tamanho_arquivo / SECTOR_SIZE) / 4;

            int next = arquivos_abertos[handle].arquivo.firstCluster;
            int anterior = arquivos_abertos[handle].arquivo.firstCluster;

            while (novo_num_clusters > 0) {
                anterior = next;
                next = encontra_proximo_setor(anterior);
                novo_num_clusters--;
            }

            if (insere_entrada_FAT(anterior, 0x00000000) < 0) {
                return -1;
            }

            if (insere_entrada_FAT(anterior, 0xFFFFFFFF) < 0) {
                return -1;
            }

            arquivos_abertos[handle].arquivo.bytesFileSize = (DWORD) novo_tamanho_arquivo;

            int posicao_dir = busca_posicao_entrada(arquivos_abertos[handle].arquivo.name,
                                                    arquivos_abertos[handle].diretorio_pai.firstCluster);

            if (posicao_dir < 0) {
                return -1;
            }

            if (insere_entrada(arquivos_abertos[handle].diretorio_pai.firstCluster, arquivos_abertos[handle].arquivo,
                               posicao_dir) < 0) {
                return -1;
            }

            return 0;
        }
    }
}


int seek2(FILE2 handle, unsigned int offset) {

    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (arquivos_abertos[handle].aberto == 1) {
            if (offset <= arquivos_abertos[handle].arquivo.bytesFileSize) {
                arquivos_abertos[handle].current_pointer = offset;
                return 0;
            } else {
                arquivos_abertos[handle].current_pointer = arquivos_abertos[handle].arquivo.bytesFileSize + 1;
                printf("Como o seek informado eh maior do que o tamanho do arquivo, o current pointer foi posicionado apos"
                               " a ultima posicao do arquivo: %d\n", arquivos_abertos[handle].current_pointer);
                return 0;
            }
        }
    }
    return -1;
}


int mkdir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    char inicio[strlen(pathname) + strlen(current_path) + 2];
    char final[strlen(pathname) + strlen(current_path) + 2];

    if (divide_caminho(pathname, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname, TYPEVAL_DIRETORIO);

    if (record.TypeVal == TYPEVAL_DIRETORIO) {
        printf("ERRO: O diretorio ja existe.\n");
        return -3;
    }

    diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio, TYPEVAL_DIRETORIO);

    if (diretorio_pai.TypeVal == TYPEVAL_DIRETORIO) { //Pode criar diretório

        int posicao_FAT = busca_pos_livre_FAT();

        if (posicao_FAT >= 0) {

            int posicao_dir = busca_entrada_livre_dir(diretorio_pai.firstCluster);

            if (posicao_dir >= 0) {

                if (insere_entrada_FAT(posicao_FAT, 0xFFFFFFFF) < 0) {
                    return -4;
                }

                struct t2fs_record novo;

                strcpy(novo.name, final);
                novo.firstCluster = (DWORD) posicao_FAT;
                novo.TypeVal = TYPEVAL_DIRETORIO;
                novo.bytesFileSize = 1024;

                if (insere_entrada(diretorio_pai.firstCluster, novo, posicao_dir) < 0) {
                    return -5;
                }

                strcpy(novo.name, ".");
                if (insere_entrada(novo.firstCluster, novo, 0) < 0) {
                    return -6;
                }

                strcpy(diretorio_pai.name, "..");
                if (insere_entrada(novo.firstCluster, diretorio_pai, 1) < 0) {
                    return -7;
                }

                return 0;
            }

        }
    }
    return -8;
}

int rmdir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    char inicio[strlen(pathname) + strlen(current_path) + 2];
    char final[strlen(pathname) + strlen(current_path) + 2];

    if (divide_caminho(pathname, inicio, final) < 0) {
        return -1;
    }

    struct t2fs_record diretorio_pai;
    struct t2fs_record record;

    record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname, TYPEVAL_DIRETORIO);

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

            diretorio_pai = compara_nomes(SUPERBLOCO.RootDirCluster, inicio, TYPEVAL_DIRETORIO);

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

    if (first_time) {
        inicializa();
    }

    struct t2fs_record current;

    /* Busca e verifica a existência do diretório informado por 'pathname' */
    current = compara_nomes(SUPERBLOCO.RootDirCluster, pathname, TYPEVAL_DIRETORIO);

    /* Caso a entrada encontrada é um diretório, formata o caminho e o insere
     * no diretório atual */
    if (current.TypeVal == TYPEVAL_DIRETORIO) {
        char caminho_formatado[strlen(pathname) + strlen(current_path) + 2];
        formata_caminho(pathname, caminho_formatado);
        strcpy(current_path, caminho_formatado);
        return 0;
    }

    return -1;
}

int getcwd2(char *pathname, int size) {

    if (first_time) {
        inicializa();
    }

    /* Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro,
     * caso contrário copia o diretório atual para o endereço de memória indicada por pathname */
    if (size < strlen(current_path)) {
        return -1;
    } else {
        strcpy(pathname, current_path);
        return 0;
    }
}

DIR2 opendir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    /* Busca por um handle disponível no array de diretórios abertos */
    DIR2 handle = busca_pos_array_dir();
    struct t2fs_record record;

    /* Se handle está no intervalo possível, busca e verifica a existência do diretório informado,
     * caso exista, o insere no array de diretórios abertos e marca como aberto. */
    if (handle >= 0 && handle <= MAX_ABERTOS) {
        record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname, TYPEVAL_DIRETORIO);
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
                    if (record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_DIRETORIO) {
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

    /* Se o handle informado está dentro do intervalo possível para diretórios abertos
     * e o diretório com o handle informado está aberto, marca diretório como fechado */
    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (diretorios_abertos[handle].aberto == 1) {
            diretorios_abertos[handle].aberto = 0;
            return 0;
        }
    }

    return -1;

}
