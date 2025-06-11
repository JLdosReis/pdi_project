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
#include <stdarg.h>
#include "imagelib.h"

#define PER_LINE 16

/*-------------------------------------------------------------------------
 * Image allocation and free routines
 *   nr = number of rows
 *   nc = number of columns
 *   ml = max gray level
 *   tp = type of image
 *-------------------------------------------------------------------------*/
image img_create(int nr, int nc, int ml, int tp)
{
    image img = malloc(sizeof(image *));
    img->px = malloc(nr * nc * sizeof(int));
    img->nr = nr;
    img->nc = nc;
    img->ml = ml;
    img->tp = tp;
    return img;
}

image img_clone(image In)
{
    return img_create(In->nr, In->nc, In->ml, In->tp);
}

void img_free(image Im)
{
    free(Im->px);
    free(Im);
}

/*-------------------------------------------------------------------------
 * Define input and output image name
 * Params:
 *   name = image name file
 *   in = input image name
 *   out = output image name
 *   tpIn, tpOut = image type (BW, GRAY, COLOR)
 *-------------------------------------------------------------------------*/

void img_name(char *name, char *in, char *out){
    int i = 0;
    while (name[i] != '\0' && name[i] != '.'){
        in[i] = name[i];
        out[i] = name[i];
        i++;
    }
    in[i] = '\0';
    strcat(in, ".pgh");
    out[i] = '\0';
    strcat(out, "-result.pgm");
}

/*-------------------------------------------------------------------------
 * Display image information
 *   name = file name image
 *   nr = number of rows
 *   nc = number os columns
 *   ml = max grayscale level
 *   tp = image type (1, 2 ou 3)
 *-------------------------------------------------------------------------*/
void img_info(char *name, image img)
{
    printf("\nImage Informations:");
    printf("\n--------------------------\n");
    printf("Image file name.............: %s \n", name);
    printf("Image type..................: P%d\n", img->tp);
    printf("Number of rows..............: %d \n", img->nr);
    printf("Number of columns...........: %d \n", img->nc);
    printf("Max intensity level.........: %d \n\n", img->ml);
}

/*-------------------------------------------------------------------------
 * Error message
 *   str - string message control
 *   ... - parameters
 *-------------------------------------------------------------------------*/
void errormsg(char *str, ...)
{
    char format[255];
    va_list arg;
    va_start(arg, str);
    sprintf(format, "ERROR: %s\n\n", str);
    vprintf(format, arg);
    va_end(arg);
    exit(1);
}

/*-------------------------------------------------------------------------
 * Read pnm ascii image
 * Params (in):
 *   name = image file name
 *   tp = image type (BW, GRAY or COLOR)
 * Returns:
 *   image structure
 *-------------------------------------------------------------------------+*/
image img_get(char *name, int tp)
{
    char lines[100];
    int nr, nc, ml;
    image img;
    FILE *fimg;
    ERROR((fimg = fopen(name, "r")) == NULL, errormsg("File open error: <%s>", name));
    /*--- PNM = "P1" or "P2" or "P3" ---*/
    fgets(lines, 80, fimg);
    ERROR((lines[0] != 'P' || (lines[1] != tp + '0')), errormsg("File type image error: <%s>", name));
    /*--- Comment lines ---*/
    fgets(lines, 80, fimg);
    while (strchr(lines, '#'))
        fgets(lines, 80, fimg);
    sscanf(lines, "%d %d", &nc, &nr);
    if (tp != BW)
        fscanf(fimg, "%d", &ml);
    else
        ml = 1;
    ERROR(nc == 0 || nr == 0 || ml == 0, errormsg("Image dimensions error: <%s>", name));
    img = img_create(nr, nc, ml, tp);
    ERROR(!img, errormsg("Image allocation error: %s\n\n img_get routine", name));
    for (int i = 0; i < nr * nc; i++)
        if (tp != COLOR)
        {
            int k;
            fscanf(fimg, "%d", &k);
            ERROR(k > ml, errormsg("Max pixel intensity in the image error: <%s>", name));
            img->px[i] = k;
        }
        else
        {
            int r, g, b;
            fscanf(fimg, "%d %d %d", &r, &g, &b);
            ERROR(r > ml || r < 0 || g > ml || g < 0 || b > ml || b < 0,
                  errormsg("Max intensity of color in the image error: <%s>", name));
            img->px[i] = (r << 16) + (g << 8) + b;
        }
    fclose(fimg);
    img_info(name, img);
    return img;
}

void img_put(image img, char *name, int tp) {
    FILE *fimg;
    ERROR((fimg = fopen(name, "wt")) == NULL, 
          errormsg("Image creation error: <%s>", name));
    
    // Cabeçalho
    fprintf(fimg, "P%d\n", tp);
    fprintf(fimg, "# Converted from PGH\n");
    fprintf(fimg, "%d %d\n", img->nc, img->nr);
    if (tp != BW) fprintf(fimg, "%d\n", img->ml);

    // Dados da imagem
    for (int i = 0; i < img->nr; i++) {
        for (int j = 0; j < img->nc; j++) {
            fprintf(fimg, "%d ", img->px[i * img->nc + j]);
        }
        fprintf(fimg, "\n");  // Nova linha após cada linha da imagem
    }
    fclose(fimg);
}