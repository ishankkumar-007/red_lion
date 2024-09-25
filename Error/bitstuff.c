#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX 100

char* bit_stuffing(char* data)
{
    int size = 0;
    char* bit_stuffed_data = (char*)calloc(MAX, sizeof(char));
    int consecutive_ones = 0;
    int i=0;
    for (i=0; data[i]!='\0'; i++)
    {
        if (data[i] == '0')
        {
            consecutive_ones = 0;
            bit_stuffed_data[size++] = data[i];
        }
        else if (data[i] == '1')
        {
            consecutive_ones += 1;
            bit_stuffed_data[size++] = data[i];
            if (consecutive_ones == 5)
            {
                consecutive_ones = 0;
                bit_stuffed_data[size++] = '0';
            }
        }
    }
    bit_stuffed_data[size++] = '\0';

    char flag[] = "01111110\0";
    char* final = (char*)calloc(MAX, sizeof(char));
    strcpy(final, flag);
    strcat(final, bit_stuffed_data);
    strcat(final, flag);
    return final;

    return bit_stuffed_data;
}

char* bit_destuffing(char* d)
{
    // to remove flag from start and end
    char* data = (char*)calloc(MAX, sizeof(char));
    strncpy(data, d+8, strlen(d) - 8 - 8);
    // memcpy(data, d+8, strlen(d)-8-8);

    int size = 0;
    char* real_data = (char*)calloc(MAX, sizeof(char));
    int consecutive_ones = 0;
    int i=0;
    for (i=0; data[i]!='\0'; i++)
    {
        if (data[i] == '0')
        {
            if (consecutive_ones == 5)
            {
                consecutive_ones = 0;
                continue;
            }
            consecutive_ones = 0;
            real_data[size++] = data[i];
        }
        else if (data[i] == '1')
        {
            consecutive_ones += 1;
            real_data[size++] = data[i];
        }
    }
    real_data[size++] = '\0';

    return real_data;
}

int main()
{
    char data[MAX];
    printf("Enter binary string: ");
    fgets(data, MAX, stdin);
    data[strcspn(data, "\n")] = '\0';


    char* bit_stuffed_data =  bit_stuffing(data);
    printf("Bit stuffed data: ");
    fputs(bit_stuffed_data, stdout); 
    printf("\n\n");


    char* real_data =  bit_destuffing(bit_stuffed_data);
    printf("Real data (destuffed): ");
    fputs(real_data, stdout); 
    printf("\n\n");

    return 0;
}