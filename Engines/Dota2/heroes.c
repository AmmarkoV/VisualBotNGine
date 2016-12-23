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
       fprintf(stderr,"#%u - `%s` - %0.2f - %0.2f - %0.2f \n",
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
  char nameC[256];
  char nameD[256];

  struct InputParserC * ipc = InputParser_Create(1024,2);
  InputParser_SetDelimeter(ipc,0,' ');
  InputParser_SetDelimeter(ipc,1,' ');

  fprintf(stderr,"Splitting input , delimiters `%c` `%c` \n" , InputParser_GetDelimeter(ipc,0) , InputParser_GetDelimeter(ipc,1) );
  InputParser_SeperateWords(ipc,raw,0);

    unsigned int heroNum=0;
    unsigned int i=0,c=0;
    while (i<1000)
    {
     //fprintf(stderr,"hero %u , place %u \n" , heroNum , i);
     InputParser_GetWord(ipc,i+0,nameA,256); //fprintf(stderr,"nameA=%s (%0.2f)"   , nameA, atof(nameA));
     InputParser_GetWord(ipc,i+1,nameB,256); //fprintf(stderr,"nameB=%s (%0.2f)"   , nameB, atof(nameB));
     InputParser_GetWord(ipc,i+2,nameC,256); //fprintf(stderr,"nameC=%s (%0.2f)"   , nameC , atof(nameC));
     InputParser_GetWord(ipc,i+3,nameD,256); //fprintf(stderr,"nameD=%s (%0.2f) \n " , nameD , atof(nameD));


     if (atof(nameD)==0) { i=i+3; snprintf(hl->heroList[heroNum].name,128,"%s %s %s %s",nameA,nameB,nameC,nameD); } else
     if (atof(nameC)==0) { i=i+2; snprintf(hl->heroList[heroNum].name,128,"%s %s %s",nameA,nameB,nameC); }          else
     if (atof(nameB)==0) { i=i+1; snprintf(hl->heroList[heroNum].name,128,"%s %s",nameA,nameB); }                   else
                         { snprintf(hl->heroList[heroNum].name,128,"%s",nameA); }



      char val[256];

      InputParser_GetWord(ipc,i+1,val,256);
      hl->heroList[heroNum].globalWinRate  = atof(val); // InputParser_GetWordFloat(ipc,i+1);

      InputParser_GetWord(ipc,i+2,val,256);
      hl->heroList[heroNum].globalPickRate = atof(val); // InputParser_GetWordFloat(ipc,i+2);

      InputParser_GetWord(ipc,i+3,val,256);
      hl->heroList[heroNum].KDA            = atof(val); // InputParser_GetWordFloat(ipc,i+3);
      i+=4;

      ++heroNum;
      if (HERO_NUMBER<=heroNum) { break; }
    }

   printHeroStats(hl);

  InputParser_Destroy(ipc);


}
