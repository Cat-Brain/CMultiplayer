#include "Include.h"

void GetStrInput(char* result, uint maxLength)
{
    fgets(result, maxLength + 1, stdin);
    for (uint i = 0; i < maxLength + 1; i++)
        if (result[i] == '\n' || i == maxLength)
        {
            result[i] = '\0';
            break;
        }
}