#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULTBUFFERSIZE 64

// Type definitions
typedef struct{
    char *name;
    double price;
    double cps;
    double cpsPerDollar;
} GENERATOR;

// Function definitions
int resPtrChar(char *, int *);
int handleProperty(char *, int *, int *, GENERATOR *, int *, int *);
int handleCharacter(char, char *, int *, int *);
int expandBuffer(char *, int *);
int sortGenerators(GENERATOR *, int);

// Main function
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./cookies [input] [output].\n");
        return 1;
    }
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        fprintf(stderr, "Could not open input file.\n");
        return 1;
    }

    GENERATOR *generators = (GENERATOR *)malloc(sizeof *generators);
    int generatorNb = 1;

    int bufferMaxSize = DEFAULTBUFFERSIZE;
    char *buffer = (char *)calloc(bufferMaxSize, sizeof *buffer);
    if (generators == NULL || buffer == NULL)
    {
        fprintf(stderr, "Error allocating initial memory.\n");
        return 2;
    }
    buffer[bufferMaxSize - 1] = 0;

    int currentGenerator = 0;
    int currentProperty = 0;
    int currentCharIndex = 0;

    char currentByte;
    while (fread(&currentByte, sizeof currentByte, 1, input) == 1) // Iterate over every byte in the input file
    {
        // Allocate space in memory for a new buffer on every new line
        if (currentGenerator > 0 && currentProperty == 0 && currentCharIndex == 0)
        {
            generators = (GENERATOR *)realloc(generators, (currentGenerator + 1) * sizeof(GENERATOR));
            generatorNb++;
        }
        /*fprintf(stderr, "Generator: %i, Property: %i\n", currentGenerator, currentProperty);*/
        /*fprintf(stderr, "Current byte is: %i\n", currentByte);*/
        /*fprintf(stderr, "Buffer is: %s\n", buffer);*/
        if (currentByte == ',' || currentByte == '\n')
        {
            if (handleProperty(buffer, &bufferMaxSize, &currentCharIndex, generators, &currentGenerator, &currentProperty))
                return 3;
        }
        else if (isalnum(currentByte))
        {
            if (handleCharacter(currentByte, buffer, &bufferMaxSize, &currentCharIndex))
                return 4;
        }

    }
    for (int i = 0; i < generatorNb; i++)
    {
        generators[i].cpsPerDollar = generators[i].cps / generators[i].price;
    }
    if (sortGenerators(generators, generatorNb))
        return 5;
    fprintf(stderr, "%s\n", generators[0].name);
    fprintf(stderr, "%f\n", generators[0].cps);
    fprintf(stderr, "%f\n", generators[0].price);
    fprintf(stderr, "%f\n", generators[0].cpsPerDollar);
    fprintf(stderr, "%s\n", generators[1].name);
    fprintf(stderr, "%f\n", generators[1].cps);
    fprintf(stderr, "%f\n", generators[1].price);
    fprintf(stderr, "%f\n", generators[1].cpsPerDollar);

    // Free remaining memory
    for (int i = 0; i < generatorNb; i++) // Freeing the memory previously allocated in handleProperty
    {
        free(generators[i].name);
    }
    free(buffer);
    free(generators);
    fclose(input);
}

// Other functions
int handleProperty(char *buffer, int *bufferMaxSize, int *currentCharIndex, GENERATOR *generators, int *currentGenerator, int *currentProperty)
{
    // Dynamically create a string as long as the buffer
    char *newProperty = (char *)malloc(((*currentCharIndex) + 1) * sizeof(char));
    if (newProperty == NULL)
    {
        fprintf(stderr, "Error allocating newProperty.\n");
        return 1;
    }
    newProperty[*currentCharIndex] = 0;
    // Copy buffer into newly created string
    strncpy(newProperty, buffer, *currentCharIndex);
    /*fprintf(stderr, "Wrote buffer to newProperty: \"%s\".\n", newProperty);*/
    if (resPtrChar(buffer, bufferMaxSize)) // Reset buffer if necessary
        return 2;
    *currentCharIndex = 0;
    switch (*currentProperty)
    {
        // If we are handling the name
        case 0:
            // Set generator "name" to point to the new property string
            generators[*currentGenerator].name = newProperty;
            *currentProperty = 1;
            break;
        // If we are handling the price
        case 1:
            // Set generator price to the floatified newProperty string
            generators[*currentGenerator].price = atof(newProperty);
            // Free unused newProperty
            free(newProperty);
            *currentProperty = 2;
            break;
        // If we are handling the cps
        case 2:
            // Set generator cps to the floatified newProperty string
            generators[*currentGenerator].cps = atof(newProperty);
            // Free unused newProperty
            free(newProperty);
            *currentProperty = 0;
            (*currentGenerator)++;
            // Allocate space for the next generator
            break;
        default:
            fprintf(stderr, "currentProperty is outside of intended range.\n");
            return 2;
    }
    return 0;
}

int resPtrChar(char *buffer, int *bufferMaxSize)
{
    if (*bufferMaxSize != DEFAULTBUFFERSIZE)
    {
        buffer = (char *)realloc(buffer, DEFAULTBUFFERSIZE * sizeof(char));
        if (buffer == NULL)
        {
            fprintf(stderr, "Error reallocating buffer.\n");
            return 1;
        }
        *bufferMaxSize = DEFAULTBUFFERSIZE;
    }
    return 0;
}

int handleCharacter(char character, char *bufferP, int *bufferMaxSizeP, int *currentCharIndexP)
{
    if ((*currentCharIndexP) + 1 == *bufferMaxSizeP) // Increase buffer size if we are at max size
    {
        if (expandBuffer(bufferP, bufferMaxSizeP))
            return 1;
    }
    bufferP[*currentCharIndexP] = character;
    (*currentCharIndexP)++;
    return 0;
}

int expandBuffer(char *bufferP, int *bufferMaxSizeP)
{
    *bufferMaxSizeP += DEFAULTBUFFERSIZE;
    bufferP = (char *)realloc(bufferP, *bufferMaxSizeP);
    if (bufferP == NULL)
    {
        fprintf(stderr, "Error reallocating buffer.\n");
        return 1;
    }
    return 0;
}

int sortGenerators(GENERATOR *generators, int generatorNb)
{
    return 0;
}
