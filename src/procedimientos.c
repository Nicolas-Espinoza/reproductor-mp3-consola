#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sound.h"

//espinoza nicolas

struct profilesong
{
    char artistname [64];
    char songname [96];
    uint32_t audioformat;
    float Sample_Rate;
};

typedef  struct profilesong profilesong;

struct aux
{
    uint32_t label;
    uint32_t length;
};

typedef struct aux aux;

enum FormatType
{
    MonoR             = 0,
    MonoL             = 1,
    StereoSecuencial  = 2,
    StereoInterleaved = 3
};


void show_songs(char *c)                                         // 0 negro
{                                                                // 1 azul           // 9 azul claro
    FILE *f;                                                     // 2 verde          // A verde claro
    f = fopen(c,"rb");                                           // 3 agua marina    // B agua marina claro
    if(f == NULL)                                                // 4 rojo           // C rojo claro
    {                                                            // 5 purpura        // D purpura claro
        printf("No se pudo abrir el archivo:");                  // 6 amarillo       // E amarillo claro
        exit(-1);                                                // 7 blanco         // F blanco brillante
    }                                                            // 8 gris
    uint8_t Count_song;
    fread(&Count_song,sizeof (uint8_t),1,f);


    profilesong *play_list;

    aux *auxiliar;

    //declaro el vector para las canciones
    play_list = (profilesong*)malloc(sizeof (profilesong)*Count_song);
    int i;
    for (i=0 ; i<Count_song;i++)
    {
        fread    (&play_list[i].artistname,sizeof (char),64,f); //artist name
        fread      (&play_list[i].songname,sizeof (char),96,f); //song name
        fread(&play_list[i].audioformat,sizeof (uint32_t),1,f); // audio format
        fread   (&play_list[i].Sample_Rate,sizeof (float),1,f); //sample rate
    }

    //declaro el vector auxiliar guarda cada length y format de cada cancion
    auxiliar= (aux*)malloc(sizeof (float)*Count_song);

    //declaro las variables auxiliares que me ayudaran a sacar los bits
    uint32_t auxlabel  = 0xC0000000;
    uint32_t auxlength = 0x3FFFFFFF;
    //inicio sesion con el audio
    init_audio();
    if ( init_audio() == false)
    {
        printf("\n No se puede iniciar audio");
    }

    //lleno el vector con las operaciones binarias
    for(i = 0 ; i < Count_song ; i++)
    {
        auxiliar[i].label  = play_list[i].audioformat & auxlabel;
        auxiliar[i].length = play_list[i].audioformat & auxlength;
        auxiliar[i].label  = auxiliar[i].label >> 30;
    }
    printf("Desea cambiar el color del reproductor?  [Y/N]:\n ");
    char s;
    do{
        scanf("%c",&s);
    }while(s != 'y' && s!= 'Y' && s!= 'n' && s!= 'N' );

    char color[9];

    if(s == 'Y' || s == 'y')
    {
        printf("\nColores disponibles: \n");
        printf("Escribir color seguido de los colores que desee\n ");
        printf("ejemplo color 09  -Fondo Negro Fuente Azul Claro-\n");
        printf("[0] Negro\n[1] Azul\t\t [9] Azul claro\n[2] Verde\t\t [A] Verde Claro\n[3] Agua Marina "
               "   \t [B] Agua Marina claro\n[4] Rojo\t\t [C] Rojo Claro\n[5] Purpura\t\t [D] Purpura Claro\n"
               "[6] Amarillo\t\t [E] Amarillo Claro\n[7] Blanco\t\t [F] Blanco Brillante\n[8] Gris\n");
        printf("introduzca color:\n\n");
      for(i=0;i<9;i++)
      {
          scanf("%c",&color[i]);
      }
      color[i]='\0';

        system(color);
    }
    if(s== 'n' || s == 'N')
    {
      printf("Continuando la reproduccion:");
    }

    //imprimo los datos
    for(i = 0 ; i <Count_song ; i++)
    {
        printf("\n[%d] Artist Name: %s    -   Song Name:   %s  @%.0f ",i,play_list[i].artistname,play_list[i].songname,(double)play_list[i].Sample_Rate);
    }
    //ya tengo todos los datos del header, el puntero esta situado en el 1er byte del sample de la cancion 0
    //length cantidad de floats por canal


    //manipulacion del cursor (numeros expresados en bytes)
    //header                 = 672

    //bytes de  primer tema  = 4236176   //stereoSecuencial   @32000
    //lenght de primer tema  = 529438

    //bytes de segundo tema  = 5289972   //monoR              @16000
    //legth    segundo tema  = 263281

    //bytes     tercer tema  = 41011860 //stereo interleaved  @176400
    //length tercer tema     = 4465152

    //bytes cuarto tema      = 41011860 //stereo secuencial   @96000
    //length cuarto tema     = 2452584
   int select;
   printf("\n\n");
   do{
       printf("Por Favor seleccione una cancion:   ");
       scanf("%d",&select);
       if(select >= Count_song)
       {
           printf("la cancion solicitada no esta en la playlist:\n ");
       }

   }while(Count_song <= select);

   //desplazo el puntero a la cancion seleccionada
    int *vector_of_bytes;
    int Byte_of_header;
    Byte_of_header = ftell(f)-1;

    vector_of_bytes = (int*)malloc(sizeof (int)*Count_song);
    i=0;
    while (i < Count_song) {
        switch (auxiliar[i].label) {
        case MonoL:
        {
            vector_of_bytes[i]= ((int)(auxiliar[i].length*(sizeof (float)))+ Byte_of_header);
        }
            break;
        case MonoR:
        {
            vector_of_bytes[i]= ((int)(auxiliar[i].length*(sizeof (float)))+ Byte_of_header);
        }
            break;
        case StereoSecuencial:
        {
            vector_of_bytes[i]= ((int)(auxiliar[i].length*(sizeof (float))*2) + Byte_of_header);
        }
            break;
        case StereoInterleaved:
        {
            vector_of_bytes[i]= ((int)(auxiliar[i].length*(sizeof (float))*2) + Byte_of_header);
        }
            break;

        }
        i++;
    }
    int Add_byte =0;
    for(i=0 ; i < select ; i++)
    {
        Add_byte = Add_byte + vector_of_bytes[i];
    }
    //desplazando el cursor a la cancion elegida
    fseek(f,Add_byte,SEEK_CUR);

    //rescato los datos de acuerdo al format

    float *left,*right;

    switch (auxiliar[select].label) {
    case MonoL:
    {
        printf("\nReproduciendo en Formato MonoL\n");
        uint32_t size_vector;
        size_vector = auxiliar[select].length;
        left = (float*)malloc(sizeof (float)*size_vector);
        fread(left,sizeof(float),size_vector,f);
        play_audio(left,NULL,(int)size_vector,(double)play_list[select].Sample_Rate);
        while(playing_complete() == false)
        {

        }
        printf(("\nDesea Guardar la cancion seleccionada? [Y/N]\n"));
        do{
            scanf("%c",&s);
        }while(s != 'y' && s!= 'Y' && s!= 'n' && s!= 'N' );
        if(s == 'y' || s== 'Y')
        {
            FILE *g;
            g = fopen("C:\\Users\\Reaper\\Desktop\\musica\\music.raw","wb");
            if(f == NULL)
            {
               printf("\nNo se pudo acceder a la ruta de guardado...\n");
               free(left);
               free(play_list);
               free(auxiliar);
               free(vector_of_bytes);
               close_audio();
               exit(-1);
            }
            fwrite(left,sizeof (float),size_vector,g);
            fclose(g);
            printf("Guardado con Exito:\n");
         }
        else{
            printf("Cerrando programa: ....");
        }
        free(left);
        close_audio();
    }
        break;
    case MonoR:
    {
        printf("\nReproduciendo en Formato MonoR\n");
        uint32_t size_vector;
        size_vector = auxiliar[select].length;
        right = (float*)malloc(sizeof (float)*size_vector);
        fread(right,sizeof(float),size_vector,f);
        play_audio(right,NULL,(int)size_vector,(double)play_list[select].Sample_Rate);
        while(playing_complete() == false)
        {

        }
        printf(("\nDesea Guardar la cancion seleccionada? [Y/N]\n"));
        do{
            scanf("%c",&s);
        }while(s != 'y' && s!= 'Y' && s!= 'n' && s!= 'N' );
        if(s == 'y' || s== 'Y')
        {
            FILE *g;
            g = fopen("C:\\Users\\Reaper\\Desktop\\musica\\music.raw","wb");
            if(f == NULL)
            {
               printf("\nNo se pudo acceder a la ruta de guardado...\n");
               free(right);
               free(play_list);
               free(auxiliar);
               free(vector_of_bytes);
               close_audio();
               exit(-1);
            }
            fwrite(right,sizeof (float),size_vector,g);
            fclose(g);
            printf("Guardado con Exito:\n");
        }
        else{
            printf("Cerrando programa:   ");
        }
        free(right);
        close_audio();
    }
        break;
    case StereoSecuencial:
    {
        printf("\nReproduciendo en Formato Stereo Secuencial\n");
        uint32_t size_vector;
        size_vector = auxiliar[select].length; //ya que es canal stereo
        right = (float*)malloc(sizeof (float)*size_vector);
        left  = (float*)malloc(sizeof (float)*size_vector);
        fread(left,sizeof(float),size_vector,f);
        fread(right,sizeof(float),size_vector,f);
        play_audio(left,right,(int)size_vector,(double)play_list[select].Sample_Rate);
        while(playing_complete() == false)
        {

        }
        printf(("\nDesea Guardar la cancion seleccionada? [Y/N]\n"));
        do{
            scanf("%c",&s);
        }while(s != 'y' && s!= 'Y' && s!= 'n' && s!= 'N' );
        if(s == 'y' || s== 'Y')
        {
            FILE *g;
            g = fopen("C:\\Users\\Reaper\\Desktop\\musica\\music.raw","wb");
            if(f == NULL)
            {
               printf("\nNo se pudo acceder a la ruta de guardado...\n");
               free(left);
               free(right);
               free(play_list);
               free(auxiliar);
               free(vector_of_bytes);
               close_audio();
               exit(-1);
            }
            for(i=0 ; i < (int)size_vector ; i ++)
            {
                fwrite( &left[i],sizeof (float),1,g);
                fwrite(&right[i],sizeof (float),1,g);
            }
            printf("Guardado con Exito:\n");
        }
        else{
            printf("Cerrando programa:   ");
        }
        close_audio();
        free(left);
        free(right);
    }
        break;
    case StereoInterleaved:
    {
        printf("\nReproduciendo en Formato Stereo Interleaved\n");
        uint32_t size_vector;
        size_vector = (auxiliar[select].length); //ya que es canal stereo
        right = (float*)malloc(sizeof (float)*size_vector);
        left  = (float*)malloc(sizeof (float)*size_vector);
        for(i=0 ; i<(int) size_vector ; i++)
        {
            fread(&left [i], sizeof(float),1,f);
            fread(&right[i], sizeof(float),1,f);
        }
        play_audio(left,right,(int)size_vector,(double)play_list[select].Sample_Rate);
        while(playing_complete() == false)
        {

        }
        printf(("\nDesea Guardar la cancion seleccionada? [Y/N]\n"));
        do{
            scanf("%c",&s);
        }while(s != 'y' && s!= 'Y' && s!= 'n' && s!= 'N' );
        if(s == 'y' || s== 'Y')
        {
            FILE *g;
            g = fopen("C:\\Users\\Reaper\\Desktop\\musica\\music.raw","wb");
            if(f == NULL)
            {
               printf("\nNo se pudo acceder a la ruta de guardado...\n");
               free(left);
               free(right);
               free(play_list);
               free(auxiliar);
               free(vector_of_bytes);
               close_audio();
               exit(-1);
            }
            for(i=0 ; i < (int)(size_vector) ; i ++)
            {
                fwrite(&left[i],sizeof (float),1,g);
                fwrite(&right[i],sizeof (float),1,g);
            }
            printf("Guardado con Exito:\n");
        }
        else{
            printf("Cerrando programa:   ");
        }
        free(left);
        free(right);
        close_audio();
    }

        break;
    }
    free(play_list);
    free(auxiliar);
    free(vector_of_bytes);

} //end del procedimiento
