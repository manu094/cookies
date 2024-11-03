#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULTBUFFERSIZE 256

// Type definitions
typedef struct{
    char *name;
    double price;
    double cps;
} GENERATOR;

// Function definitions
int resPtrChar(char *, int *);
int handleProperty(char *, int *, GENERATOR *, int *, int *);

// Main function
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./cookies [input] [output].\n"); // NOTE: replace printf("") with fprintf(stderr, "")
        return 1;
    }
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open input file.\n");
        return 1;
    }

    GENERATOR *generators = (GENERATOR *)malloc(sizeof *generators);
    char *buffer = (char *)calloc(DEFAULTBUFFERSIZE, sizeof *buffer);
    if (generators == NULL || buffer == NULL)
    {
        printf("Error allocating initial memory.\n");
        return 2;
    }
    *buffer = '\0';
    int bufferSize = 1;
    int currentGenerator = 0;
    int currentProperty = 0;

    uint8_t currentByte;
    while (fread(&currentByte, sizeof currentByte, 1, input) == 1)
    {
        if (currentByte == ',' || currentByte == '\n')
        {
            if (handleProperty(buffer, &bufferSize, generators, &currentGenerator, &currentProperty))
            {
                return 3;
            }
        }
    }
}

// Other functions
int handleProperty(char *buffer, int *bufferSize, GENERATOR *generators, int *currentGenerator, int *currentProperty) // TODO: remove unnecessary pointers
{
    // Dynamically create a string as long as the buffer
    char *newProperty = (char *)malloc(*bufferSize * sizeof(char));
    if (newProperty == NULL)
    {
        fprintf(stderr, "Error allocating newProperty.\n");
        /*printf("Error allocating newProperty.\n");*/
        return 1;
    }
    // Copy buffer into newly created string
    strncpy(newProperty, buffer, *bufferSize);
    switch (*currentProperty)
    {
        // If we are handling the name
        case 0:
            // Set generator "name" to point to the new property string
            (generators + *currentGenerator)->name = newProperty;
            if (resPtrChar(buffer, bufferSize)) // TODO: Reset bufferSize
            {
                return 2;
            }
            *currentProperty = 1;
            break;
        // If we are handling the price
        case 1:
            // Set generator price to the floatified newProperty string
            (generators + *currentGenerator)->price = atof(newProperty);
            // Free unused newProperty and reset buffer to 0 characters for next newProperty
            free(newProperty);
            if (resPtrChar(buffer, bufferSize))
            {
                return 2;
            }
            *currentProperty = 2;
            break;
        // If we are handling the cps
        case 2:
            // Set generator cps to the floatified newProperty string
            (generators + *currentGenerator)->cps = atof(newProperty);
            // Free unused newProperty and reset buffer to 0 characters for next newProperty
            free(newProperty);
            if (resPtrChar(buffer, bufferSize))
            {
                return 2;
            }
            *currentProperty = 0;
            // Allocate space for the next generator
            (*currentGenerator)++;
            generators = (GENERATOR *)realloc(generators, *currentGenerator + 1);
            break;
        default:
            printf("currentProperty is outside of intended range.\n");
    }
    return 0;
}

int resPtrChar(char *buffer, int *bufferSize)
{
    buffer = (char *)realloc(buffer, sizeof(char));
    if (buffer == NULL)
    {
        printf("Error reallocating buffer.\n");
        return 1;
    }
    *buffer = '\0';
    *bufferSize = 1;
    return 0;
}
