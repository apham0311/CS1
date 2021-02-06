#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"
// Anh Pham

#define MAX 200

typedef struct monster
{
  char *name;
  char *element;
  int population;
} monster;

typedef struct region
{
  char *name;
  int nmonsters;
  int total_population;
  monster **monsters;
} region;

typedef struct itinerary
{
  int nregions;
  region **regions;
  int captures;
} itinerary;

typedef struct trainer
{
  char *name;
  itinerary *visits;
} trainer;

monster *createMonster(char *name, char *element, int population)
{
  int nameLength;
  int elementLength;
  nameLength = strlen(name) + 1;
  elementLength = strlen(element) + 1;

  //allocate memory for struct
  monster *newMonster = (monster *)malloc(sizeof(monster));
  newMonster->name = (char *)malloc(nameLength * sizeof(char));
  newMonster->element = (char *)malloc(elementLength * sizeof(char));

  strncpy(newMonster->name, name, nameLength);
  strncpy(newMonster->element, element, elementLength);
  newMonster->population = population;

  return newMonster;
}

monster **readMonsters(FILE *infile, int *monsterCount)
{
  monster **monstersList;
  monster *monster;
  char name[MAX];
  char element[MAX];
  int population;

  fscanf(infile, "%d %*s", monsterCount);                                           //For the first line scan in
  monstersList = (struct monster **)malloc(*monsterCount * sizeof(struct monster)); //amke array of the struct

  for (int i = 0; i < *monsterCount; i++)
  {
    fscanf(infile, "%s %s %d", name, element, &population); //scan in the name
    monstersList[i] = createMonster(name, element, population);
  }

  return monstersList;
}

// This will make a tempoary struct box  for region
// this will get infor from file and make a new region struct
region *createRegion(char *name, int numMonsters, int monsterCount, char **reMonsterName, monster **monsterList)
{
  int i;
  int j;
  int nameLength;
  int popTotal = 0;

  nameLength = strlen(name) + 1;

  // Allocates memory for  structure
  region *myRegion = (struct region *)malloc(sizeof(struct region));
  myRegion->name = (char *)malloc(nameLength * sizeof(char));
  myRegion->monsters = (struct monster **)malloc(numMonsters * sizeof(struct monster *));

  strncpy(myRegion->name, name, nameLength);
  myRegion->nmonsters = numMonsters;
  // give the struct values

  for (i = 0; i < myRegion->nmonsters; i++) //this will ve use to comapare names of monsters
  {

    for (j = 0; j < monsterCount; j++)
    {
      // Assigns values to region monsters structure
      // if/when the monster's name is found in monsterList
      if (strcmp(reMonsterName[i], monsterList[j]->name) == 0)
      {

        myRegion->monsters[i] = createMonster(monsterList[j]->name, monsterList[j]->element, monsterList[j]->population);
        myRegion->monsters[i]->population = monsterList[j]->population;
        popTotal += monsterList[j]->population;
      }
    }
  }

  myRegion->total_population = popTotal;

  //free the mem because not needed later
  for (int i = 0; i < numMonsters; i++)
    free(reMonsterName[i]);
  free(reMonsterName);

  return myRegion;
}

region **readRegions(FILE *infile, int *countRegions, monster **monsterList, int monsterCount)
{
  region **regionList;
  monster **regionMonster;
  char regionName[MAX];
  char monsterName[MAX];
  char **reMonsterName;
  int numMonsters;

  fscanf(infile, "%d %*s", countRegions);

  regionList = (struct region **)malloc(*countRegions * sizeof(struct region));

  for (int i = 0; i < *countRegions; i++)
  {

    fscanf(infile, "%s", regionName);
    fscanf(infile, "%d %*s", &numMonsters); //amount region

    reMonsterName = (char **)malloc(numMonsters * sizeof(char *));

    for (int j = 0; j < numMonsters; j++)
    {

      fscanf(infile, "%s", monsterName);
      reMonsterName[j] = (char *)malloc((strlen(monsterName) + 1) * sizeof(char));
      strcpy(reMonsterName[j], monsterName);
    }
    regionList[i] = createRegion(regionName, numMonsters, monsterCount, reMonsterName, monsterList);
  }
  return regionList;
}

region *getRegion(region **regionList, int regionAtlas)
{
  region *infoRegion = (struct region *)malloc(sizeof(struct region));
  infoRegion->monsters = (struct monster **)malloc((regionList[regionAtlas]->nmonsters) * sizeof(struct monster *));
  //memory allocation fof the struct

  //this will fill in info for the struct
  for (int i = 0; i < regionList[regionAtlas]->nmonsters; i++)
    infoRegion->monsters[i] = regionList[regionAtlas]->monsters[i];

  infoRegion->name = regionList[regionAtlas]->name;
  infoRegion->nmonsters = regionList[regionAtlas]->nmonsters;
  infoRegion->total_population = regionList[regionAtlas]->total_population;

  return infoRegion;
}

//make itenary struct
itinerary *createItinerary(int numRegions, int numCaptures, region **trainerRegionList)
{

  itinerary *newItinerary = (struct itinerary *)malloc(sizeof(struct itinerary)); //allocate memory

  newItinerary->nregions = numRegions;
  newItinerary->captures = numCaptures;
  //fill in the data

  newItinerary->regions = trainerRegionList;

  return newItinerary;
}

trainer *createTrainer(char *name, itinerary *trainerItinerary)
{

  trainer *newTrainer = (struct trainer *)malloc(sizeof(struct trainer));

  int nameLength = (strlen(name) + 1);

  //allocate memory for the trainer
  newTrainer->name = (char *)malloc(nameLength * sizeof(char));
  strcpy(newTrainer->name, name);
  newTrainer->visits = trainerItinerary;
  return newTrainer;
}

trainer **readTrainers(FILE *inFile, int *trainerCount, region **regionList, int countRegions)
{
  trainer **trainerList;
  int i;
  int *tempCaptures;
  int *tempRegions;
  char regionName[MAX], trainerName[MAX];

  fscanf(inFile, "%d %*s", trainerCount);

  // allocating memory
  trainerList = (trainer **)malloc(*trainerCount * sizeof(trainer *));
  tempCaptures = (int *)malloc(*trainerCount * sizeof(int));
  tempRegions = (int *)malloc(*trainerCount * sizeof(int));

  for (int i = 0; i < *trainerCount; i++)
  {
    fscanf(inFile, "%s", trainerName);
    fscanf(inFile, "%d %*s", &tempCaptures[i]);
    fscanf(inFile, "%d %*s", &tempRegions[i]);
    region **tempRegionList = (region **)malloc(tempRegions[i] * sizeof(region *));

    for (int j = 0; j < tempRegions[i]; j++)
    {
      fscanf(inFile, "%s", regionName);

      for (int k = 0; k < countRegions; k++)
      {
        if (strcmp(regionName, regionList[k]->name) == 0)
        {
          tempRegionList[j] = regionList[k];
        }
      }
    }
    itinerary *trainerItinerary = createItinerary(tempRegions[i], tempCaptures[i], tempRegionList);
    trainerList[i] = createTrainer(trainerName, trainerItinerary); //make a struct for the trainer
  }

  // *numCaptures = tempCaptures;
  // *numRegions = tempRegions;
  // *ttRegionNames = tempRegionNames;

  return trainerList;
}

//do math
float approxCaptures(int totalPop, int monsterPop, int trainerCapture)
{

  float percentPop = (float)monsterPop / totalPop;
  float intendCapture = percentPop * trainerCapture;

  return intendCapture;
}

// void process_inputs(monster** monsterList, int monsterCount, region** regionList,
//     int regionCount, trainer* trainerList, int trainerCount ) {
//   for(int i = 0; i < trainerCount; i++) {
//     trainerList[i]->visits=visits->regions[j] = retrieveRegion(regionList, k);
// }

//free memory
void release_memory(monster **monsterList, int monsterCount, region **regionList,
                    int regionCount, trainer **trainerList, int trainerCount)
{
  for (int i = 0; i < monsterCount; i++)
  {
    free(monsterList[i]->name);
    free(monsterList[i]->element);
    free(monsterList[i]);
  }
  free(monsterList);

  for (int i = 0; i < regionCount; i++)
  {
    for (int j = 0; j < regionList[i]->nmonsters; j++)
    {
      free(regionList[i]->monsters[j]->name);
      free(regionList[i]->monsters[j]->element);
      free(regionList[i]->monsters[j]);
    }
    free(regionList[i]->monsters);
    free(regionList[i]->name);
    free(regionList[i]);
  }

  free(regionList);

  for (int i = 0; i < trainerCount; i++)
  {
    for (int j = 0; j < trainerList[i]->visits->nregions; j++)
    {
      free(trainerList[i]->visits->regions[j]->monsters);
      free(trainerList[i]->visits->regions[j]);
    }

    free(trainerList[i]->visits->regions);
    free(trainerList[i]->visits);
    free(trainerList[i]->name);
    free(trainerList[i]);
  }

  free(trainerList);
}

int main()
{
  atexit(report_mem_leak); //for memory leak detector.
  int i, j, k;
  FILE *inFile, *outFile;
  int monsterCount;
  int regionCount;
  int trainerCount;
  monster **monsterList;
  region **regionList;
  trainer **trainerList;
  char ***tRegionNames;
  int *numRegions;
  int *numCaptures;
  float ***intendCapture;
  inFile = fopen("in.txt", "r");

  if (inFile != NULL)
  {

    monsterList = readMonsters(inFile, &monsterCount);
    regionList = readRegions(inFile, numRegions, monsterList, monsterCount);
    trainerList = readTrainers(inFile, &trainerCount, regionList, *numRegions);
    intendCapture = (float ***)malloc(trainerCount * sizeof(float **));
  }
  else
  {
    printf("Please provide correct input file");
    exit(-1);
  }
  return 0;
}
