//#define _CRT_SECURE_NO_WARNINGS
#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
	char* tmp = nextWord(file);

	while (tmp != NULL) 
	{
		hashMapPut(map, tmp, 1);
		tmp = nextWord(file);
	}
}
//Levenshteins distance was used as a means of finding words similar to improper
//user input by taking two strings of some length and returning the editing distance
//between them.
int levenshtein(char* s1, char* s2) 
{
	unsigned int s1length, s2length, x, y, lastd, oldd;
	//Two strings whose characters are to be represented in an array that will be used
	//to obtain Levenshtein distance between them.
	s1length = strlen(s1);
	s2length = strlen(s2);

	unsigned int column[s1length + 1];
	for (y = 1; y <= s1length; y++)
	 column[y] = y;

	for (x = 1; x <= s2length; x++)
	{
	 column[0] = x;

	 for (y = 1, lastd = x - 1; y <= s1length; y++)
	 {
	  oldd = column[y];
	  column[y] = MIN3(column[y] + 1, column[y - 1] + 1,
	  lastd + (s1[y - 1] == s2[x - 1] ? 0 : 1));
	  lastd = oldd;
	 }
	}
	//Edit distance from array.
	return(column[s1length]);
}

void traverse(HashMap* map, char* s1)
{
	for (int i = 0; i < map->capacity; i++)
	{
	 HashLink* link = map->table[i];
	 if (link != NULL)
	 {
	  while (link != NULL)
	  {
	   link->value = levenshtein(s1, link->key);
	   link = link->next;
	  }
	 }
	}
}

//Used to sort through hashmap after link value above is set to obtained editing distance.
//It displays the five closest word matches for user input that doesn't include words
//contained by dictionary.txt
void traversal(HashMap* map)
{
	int counterFive = 0;
	int counterLevenshtein = 1;
	while (counterFive < 5)
	{
	 for (int i = 0; i < map->capacity; i++)
	 {
	  HashLink* link = map->table[i];
	  if (link != NULL)
	  {
	   while (link != NULL && counterFive < 5)
	   {
	    if (link->value == counterLevenshtein)
		{
		 printf("Did you mean... %s\n", link->key);
		 counterFive++;
		}
	   link = link->next;
	   }
	  }
	 }
	counterLevenshtein++;
	}
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
	{
	 printf("Enter a word, or type quit to exit spellChecking program. ");
	 scanf("%s", inputBuffer);

     for (int i = 0; inputBuffer[i]; i++)
	 {
	  inputBuffer[i] = tolower(inputBuffer[i]);
	 }

	 if (strcmp(inputBuffer, "quit") == 0)
	 {
	  quit = 1;
	 }

	 else if (hashMapContainsKey(map, inputBuffer))
	 {
	  printf("The inputted word is spelled correctly! %s\n", inputBuffer);
     }

	 else
	 {
	  printf("The inputted word is spelled incorrectly!\n");
	  traverse(map, inputBuffer);
	  traversal(map);
	 }

	}
     hashMapDelete(map);
    return 0;
}
