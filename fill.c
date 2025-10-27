#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Relleno(int Ancho, int Alto, int **Mapa, int X, int Y, int ColorActual, int ColorNuevo, int *Pixeles);

int main(int argc, char const *argv[])
{
    int X;
    int Y;
    int NuevoR;
    int NuevoG;
    int NuevoB;

    clock_t t_inicio, t_final;
    double t_intervalo;

    sscanf(argv[1], "(%d,%d)", &X, &Y);
    sscanf(argv[2], "(%d,%d,%d)", &NuevoR, &NuevoG, &NuevoB);

    int NuevoColor = NuevoB | NuevoG<<8 | NuevoR<<16;

    unsigned char BytesBmp[54];
    fread(&BytesBmp, 1, 54, stdin);

    int InicioImagen = BytesBmp[10] | BytesBmp[11]<<8 | BytesBmp[12]<<16 | BytesBmp[13]<<24;
    int PixelxFila = BytesBmp[18] | BytesBmp[19]<<8 | BytesBmp[20]<<16 | BytesBmp[21]<<24;
    int AlturaPixelxColumna = BytesBmp[22] | BytesBmp[23]<<8 | BytesBmp[24]<<16 | BytesBmp[25]<<24;

    int PaddingxFila = (4 - ((PixelxFila * 3) % 4)) % 4;

    // Declaración de Mapa como fila x columna
    int **Mapa = malloc(sizeof(int*) * AlturaPixelxColumna);
    for (int y = 0; y < AlturaPixelxColumna; y++)
    {
        Mapa[y] = malloc(sizeof(int) * PixelxFila);
    }

    unsigned char B,G,R;
    fseek(stdin, InicioImagen, SEEK_SET);
    for (int y = AlturaPixelxColumna - 1; y >= 0; y--) {
        for (int x = 0; x < PixelxFila; x++) {
            fread(&B, 1, 1, stdin);
            fread(&G, 1, 1, stdin);
            fread(&R, 1, 1, stdin);
            Mapa[y][x] = B | G<<8 | R<<16;
        }
        fseek(stdin, PaddingxFila, SEEK_CUR);
    }

    Y = AlturaPixelxColumna - 1 - Y;
    int ColorActual = Mapa[Y][X];
    int Pixeles = 0;

    t_inicio = clock();
    Relleno(PixelxFila, AlturaPixelxColumna, Mapa, X, Y, ColorActual, NuevoColor, &Pixeles);
    t_final = clock();
    t_intervalo = (double)(t_final - t_inicio) / CLOCKS_PER_SEC;

    unsigned char temp;

    fwrite(BytesBmp, 1, 54, stdout);
    for (int y = AlturaPixelxColumna - 1; y >= 0; y--) {
        for (int x = 0; x < PixelxFila; x++) {
            temp = Mapa[y][x] & 0xFF;
            fwrite(&temp, 1, 1, stdout);
            temp = (Mapa[y][x] >> 8) & 0xFF;
            fwrite(&temp, 1, 1, stdout);
            temp = (Mapa[y][x] >> 16) & 0xFF;
            fwrite(&temp, 1, 1, stdout);
        }
        for (int p = 0; p < PaddingxFila; p++) {
            temp = 0;
            fwrite(&temp, 1, 1, stdout);
        }
    }

    printf("Tiempo: %.10f\nPixeles coloreados: %d", t_intervalo, Pixeles);

    for (int y = 0; y < AlturaPixelxColumna; y++)
        free(Mapa[y]);
    free(Mapa);

    return 0;
}

void Relleno(int Ancho, int Alto, int **Mapa, int X, int Y, int ColorActual, int ColorNuevo, int *Pixeles)
{
    if (X < 0 || X >= Ancho || Y < 0 || Y >= Alto || Mapa[Y][X] != ColorActual)
        return;

    Mapa[Y][X] = ColorNuevo;
    (*Pixeles)++;
    Relleno(Ancho, Alto, Mapa, X-1, Y, ColorActual, ColorNuevo, Pixeles);
    Relleno(Ancho, Alto, Mapa, X, Y+1, ColorActual, ColorNuevo, Pixeles);
    Relleno(Ancho, Alto, Mapa, X+1, Y, ColorActual, ColorNuevo, Pixeles);
    Relleno(Ancho, Alto, Mapa, X, Y-1, ColorActual, ColorNuevo, Pixeles);
}
