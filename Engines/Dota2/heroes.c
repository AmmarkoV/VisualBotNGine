#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heroes.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


char * astringReadFileToMemory(const char * filename,unsigned int *length )
{
    *length = 0;
    FILE * pFile = fopen ( filename , "rb" );

    if (pFile==0)
    {
        fprintf(stderr,RED "astringReadFileToMemory failed\n" NORMAL);
        fprintf(stderr,RED "Could not read file %s \n" NORMAL,filename);
        return 0;
    }

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    unsigned long lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    char * buffer = (char*) malloc (sizeof(char)*(lSize+1));
    if (buffer == 0 )
    {
        fprintf(stderr,RED "Could not allocate enough memory for file %s \n" NORMAL,filename);
        fclose(pFile);
        return 0;
    }

    // copy the file into the buffer:
    size_t result = fread (buffer,1,lSize,pFile);
    if (result != lSize)
    {
        free(buffer);
        fprintf(stderr,RED "Could not read the whole file onto memory %s \n" NORMAL,filename);
        fclose(pFile);
        return 0;
    }

    /* the whole file is now loaded in the memory buffer. */

    // terminate
    fclose (pFile);

    buffer[lSize]=0; //Null Terminate Buffer!
    *length = (unsigned int) lSize;
    return buffer;
}



int printHeroStats(struct allHeroes * hl)
{
 unsigned int i=0;
 for (i=0; i<HERO_NUMBER; i++)
    {
       fprintf(stderr,"#%u - %s - %0.2f - %0.2f - %0.2f \n",
               i,
               hl->heroList[i].name,
               hl->heroList[i].globalWinRate,
               hl->heroList[i].globalPickRate,
               hl->heroList[i].KDA
               );
    }

}


int updateHeroStats( struct allHeroes * hl)
{
  //https://www.dotabuff.com/heroes/winning
  unsigned int length=0;
  char * raw = astringReadFileToMemory("online/winclean3", &length );
  char nameA[256];
  char nameB[256];

  struct InputParserC * ipc = InputParser_Create(1024,1);
  InputParser_SetDelimeter(ipc,0,' ');

   InputParser_SeperateWords(ipc,raw,0);

    unsigned int heroNum=0;
    unsigned int i=0,c=0;
    while (i<1000)
    {
     fprintf(stderr,"hero %u , place %u \n" , heroNum , i);
     InputParser_GetWord(ipc,i,nameA,256);
     c=i+1;
     InputParser_GetWord(ipc,c,nameB,256);
     if (InputParser_GetWordFloat(ipc,c)==0)
        {
         snprintf(hl->heroList[heroNum].name,128,"%s %s",nameA,nameB);
         ++c;
        } else
        {
          snprintf(hl->heroList[heroNum].name,128,"%s",nameA);
        }
      hl->heroList[heroNum].globalWinRate  = InputParser_GetWordFloat(ipc,c+1);
      hl->heroList[heroNum].globalPickRate = InputParser_GetWordFloat(ipc,c+2);
      hl->heroList[heroNum].KDA      = InputParser_GetWordFloat(ipc,c+3);
      ++heroNum;
      c+=3;
      i=c;
      if (HERO_NUMBER<=heroNum) { break; }
    }

   printHeroStats(hl);

  InputParser_Destroy(ipc);


}
