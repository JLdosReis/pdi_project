/*=============================================================
* UNIFAL = Universidade Federal de Alfenas .
* BACHARELADO EM CIENCIA DA COMPUTACAO.
* Trabalho: Decodificador do formato PGH
* Professor: Luiz Eduardo da Silva
* Aluno: José Luiz dos Reis Neto
* Data: 08/06/2025
*=============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imagelib.h"

#define MAXNODE 511

typedef struct{
    int nr, nc, ml;
    int *histogram;
    
} *imagePGH;

typedef struct{
    int freq;    
    int father;  
    int left;
    int right;
} nodetype;

nodetype nodes[MAXNODE];

int pqmindelete(int *rootnodes)
{
    int aux;
    aux = *rootnodes;
    *rootnodes = nodes[*rootnodes].father;
    return aux;
}

int pqinsert(int *rootnodes, int i)
{
    int ant, aux;
    aux = *rootnodes;
    ant = -1;
    while (aux != -1 && nodes[aux].freq <= nodes[i].freq)
    {
        ant = aux;
        aux = nodes[aux].father;
    }
    if (ant == -1)
    {
        nodes[i].father = *rootnodes;
        *rootnodes = i;
    }
    else
    {
        nodes[i].father = nodes[ant].father;
        nodes[ant].father = i;
    }
    return 0;
}

int buildTree(int *h, int mn)
{
    int i, p, p1, p2, rootnodes = -1;
    for (i = 0; i < mn + 1; i++)
    {
        nodes[i].freq = h[i];
        nodes[i].left = -1;
        nodes[i].right = -1;
        pqinsert(&rootnodes, i);
    }
    for (p = mn + 1; p < 2 * (mn + 1) - 1; p++)
    {
        p1 = pqmindelete(&rootnodes);
        p2 = pqmindelete(&rootnodes);
        nodes[p1].father = p;
        nodes[p2].father = p;
        nodes[p].freq = nodes[p1].freq + nodes[p2].freq;
        nodes[p].left = p1;
        nodes[p].right = p2;
        pqinsert(&rootnodes, p);
    }
    puts("\n");
    return pqmindelete(&rootnodes);
}

image read_pgh(char *name){
    int nr, nc, ml;
    imagePGH imgIn = malloc(sizeof(*imgIn));
    FILE *fimg;
    char line[100];

    ERROR((fimg = fopen(name, "rb")) == NULL, errormsg("Erro ao abrir a imagem: <%s>", name));

    //lê ph
    fgets(line, 80, fimg);
    ERROR((line[0] != 'P' || line [1] != 'H'), errormsg("Tipo de imagem inválida: <%s>", name));
    
    fgets(line, sizeof(line), fimg);
    while (line[0] == '#'){
        fgets(line, sizeof(line), fimg);
    }
    //lê colunas e linhas
    sscanf(line, "%d %d", &nc, &nr);

    //lê nivel maximo
    fgets(line, 80, fimg);
    sscanf(line, "%d", &ml);
    ERROR(nc == 0 || nr == 0 || ml == 0, errormsg("Dimensões da imagem inválidas: <%s>", name));
    imgIn->nr = nr;
    imgIn->nc = nc;
    imgIn->ml = ml;

    //histograma
    int *hist = malloc((ml + 1) * sizeof(int));
    for(int i = 0; i <= ml; i++){
        int val = 0;
        for(int j = 0; j < sizeof(int); j++){
            int byte = fgetc(fimg);
            if(byte == EOF){
                errormsg("Erro ao ler arquivo: <%s>", name);
                exit(1);
            }
            val |= (byte << (8 * j));
        }
        hist[i] = val;
    }
    imgIn->histogram = hist;

    //construindo a árvore
    int root = buildTree(hist, ml);
    image img = img_create(nr, nc, ml, GRAY);

    int current_node = root;
    int pos = 0;
    unsigned char byte;

    while(pos < nr * nc && fread(&byte, 1, 1, fimg) == 1){
        for(int bit = 7; bit >= 0; bit--){
            int current_bit = (byte >> bit) & 1;
            if(current_bit){
                current_node = nodes[current_node].right;
            } else {
                current_node = nodes[current_node].left;
            }
            if(nodes[current_node].left == -1 && nodes[current_node].right == -1){
                img->px[pos++] = current_node;
                current_node = root;
            }
        }
    }
    if(pos != nr * nc){
        errormsg("Dados incompletos: esperados %d pixels, decodificados %d", nr * nc, pos);
        exit(1);
    }
    fclose(fimg);
    free(hist);
    free(imgIn);
    return img;
}

void msg(char *s){
    printf("\nEncoding files in PGH format");
    printf("”\n===============================");
    printf("\nUsage: %s image-name[.pgh]\n\n", s);
    printf("image-name[.pgh] is image file in pgh format \n\n");
    exit(1);
}

int main (int argc, char *argv[]){
    char nameIn[100], nameOut[100], cmd[110];
    image In;

    if(argc < 2)
    msg(argv[0]);

    //define input/output file names
    img_name(argv[1], nameIn, nameOut);

    //read image PGH
    In = read_pgh(nameIn);

    //save image PGM
    img_put(In, nameOut, GRAY);

    //show image
    sprintf(cmd, "eog %s &", nameOut);
    puts(cmd);
    system(cmd);
    img_free(In);
    return 0;
}