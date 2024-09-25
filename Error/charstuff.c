#include <stdio.h>
#include <string.h>

void characterStuffing(char *originalData, char *stuffedData, char startChar, char endChar, char escapeChar)
{
    int originalLength = strlen(originalData);
    int stuffedLength = 0;

    // Add the start character
    stuffedData[stuffedLength++] = startChar;

    for (int i = 0; i < originalLength; i++)
    {
        if (originalData[i] == startChar || originalData[i] == endChar || originalData[i] == escapeChar)
        {
            stuffedData[stuffedLength++] = escapeChar;
        }
        stuffedData[stuffedLength++] = originalData[i];
    }

    // Add the end character
    stuffedData[stuffedLength++] = endChar;

    stuffedData[stuffedLength] = '\0';
}

void characterDestuffing(char *stuffedData, char *originalData, char startChar, char endChar, char escapeChar)
{
    int stuffedLength = strlen(stuffedData);
    int originalLength = 0;

    // Skip the start character
    int i = 1; // Start from the character after startChar

    while (i < stuffedLength - 1) // Stop before the last character (endChar)
    {
        if (stuffedData[i] == escapeChar)
        {
            i++; // Skip the escape character
        }
        originalData[originalLength++] = stuffedData[i++];
    }

    originalData[originalLength] = '\0';
}

int main()
{
    char originalData[100] = "Hello World!";
    char stuffedData[200];
    char destuffedData[100];

    char startChar = '<';
    char endChar = '>';
    char escapeChar = '/';

    printf("Enter the Original Data: ");
    scanf("%[^\n]", originalData);

    characterStuffing(originalData, stuffedData, startChar, endChar, escapeChar);
    printf("Stuffed Data: %s\n", stuffedData);

    characterDestuffing(stuffedData, destuffedData, startChar, endChar, escapeChar);
    printf("Destuffed Data: %s\n", destuffedData);

    return 0;
}
