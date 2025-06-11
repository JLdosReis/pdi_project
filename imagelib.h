/*=============================================================
* UNIFAL = Universidade Federal de Alfenas .
* BACHARELADO EM CIENCIA DA COMPUTACAO.
* Trabalho: Decodificador do formato PGH
* Professor: Luiz Eduardo da Silva
* Aluno: José Luiz dos Reis Neto
* Data: 08/06/2025
*=============================================================*/

#define CREATOR "# CREATOR: Image Processing using C-Ansi - ByDu\n"

typedef struct
{
    int *px;        // pixels vector
    int nr, nc, ml; // nr = n.rows, nc = n.columns, ml = max level
    int tp;         // tp = type
} * image;

#define ERROR(x, y) \
    if (x)          \
    y

enum
{
    BW = 1,
    GRAY,
    COLOR
};

#if defined(_WIN32) || defined(__WIN64__) || defined(__CYGWIN__)
#define VIEW "../utils/i_view32"
#elif defined(__linux__)
#define VIEW "eog"
#endif

void errormsg(char *, ...);
image img_create(int, int, int, int);
image img_clone(image);
void img_free(image);
void img_name(char *, char *, char *);
image img_get(char *, int);
void img_put(image, char *, int);