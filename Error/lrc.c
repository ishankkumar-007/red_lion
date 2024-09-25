#include <stdio.h>
#include <string.h>

#define BYTE_SIZE 8
#define NUM_BYTES 4

void calculate_lrc_xor(char data[NUM_BYTES][BYTE_SIZE + 1], char lrc[BYTE_SIZE + 1])
{
    // for (int i = 0; i < BYTE_SIZE; i++) {
    //     lrc[i] = data[0][i];
    //     for (int j = 1; j < NUM_BYTES; j++) {
    //         lrc[i] ^= data[j][i];
    //     }
    // }
    // lrc[BYTE_SIZE] = '\0';

    for (int i = 0; i < BYTE_SIZE; i++)
    {
        int count = 0;
        for (int j = 0; j < NUM_BYTES; j++)
        {
            if (data[j][i] == '1')
            {
                count++;
            }
        }
        // For even parity
        lrc[i] = (count % 2 == 0) ? '0' : '1';
    }
    lrc[BYTE_SIZE] = '\0';
}

void check_lrc_for_errors(char sent_data[NUM_BYTES][BYTE_SIZE + 1], char received_data[NUM_BYTES][BYTE_SIZE + 1])
{
    char sent_lrc[BYTE_SIZE + 1];
    char received_lrc[BYTE_SIZE + 1];

    calculate_lrc_xor(sent_data, sent_lrc);

    printf("\nSent LRC : ");

    for (int i = 0; sent_lrc[i] != '\0'; i++)
    {
        printf("%c", sent_lrc[i]);
    }


    // printf("\nCalculating LRC at receiver end : \n");

    calculate_lrc_xor(received_data, received_lrc);

    printf("\nReceived LRC : ");

    for (int i = 0; received_lrc[i] != '\0'; i++)
    {
        printf("%c", received_lrc[i]);
    }

    if (strcmp(sent_lrc, received_lrc) != 0)
    {
        printf("\nError detected (LRC mismatch).\n");
    }
    else
    {
        printf("\nNo error detected in the received data block (LRC matches).\n");
    }
}

int main()
{
    char sent_data[NUM_BYTES][BYTE_SIZE + 1];
    char received_data[NUM_BYTES][BYTE_SIZE + 1];
    char received_lrc[BYTE_SIZE + 1];

    printf("Enter the sent data (4 bytes of 8 bits each):\n");
    for (int i = 0; i < NUM_BYTES; i++)
    {
        printf("Enter 8-bit data for byte %d: ", i + 1);
        scanf("%s", sent_data[i]);
    }

    printf("Enter the received data (4 bytes of 8 bits each):\n");
    for (int i = 0; i < NUM_BYTES; i++)
    {
        printf("Enter 8-bit data for byte %d: ", i + 1);
        scanf("%s", received_data[i]);
    }

    check_lrc_for_errors(sent_data, received_data);

    return 0;
}
