// Ishank Kumar
// CS22B1043

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int size = 0;

char xor(char a, char b)
{
    return (a==b)? '0' : '1' ;
}

char* crc_generator(char* data, char* divisor)
{
    size = 0;
    char* crc = (char*)calloc(8, sizeof(char));
    int divisor_len = strlen(divisor); // i.e. 8  // strlen ignores '\0' but does not ignore '\n'

    char appended_data[60];
    strcpy(appended_data, data);
    // append divisor_len-1 0's at the end
    int i = strlen(appended_data);  // this index stores '\0'
    for (int j=0; j<divisor_len-1; j++)
    {
        appended_data[i++] = '0';
    }
    appended_data[i++] = '\0';

    // divide
    int data_len = strlen(data);
    for (int i=0; i < data_len; i++)
    {
        if (appended_data[i] == '1') 
        {
            for (int j = 0; j < divisor_len; j++) 
            {
                appended_data[i + j] = xor(appended_data[i + j], divisor[j]);
            }
        }
    }

    // 7 bit remainder
    strncpy(crc, &appended_data[data_len], divisor_len - 1);
    crc[divisor_len - 1] = '\0';
    return crc;
}

void crc_checker(char* data, char* divisor)
{
    char* remainder = (char*)calloc(8, sizeof(char));
    int divisor_len = strlen(divisor); // i.e. 8  // strlen ignores '\0' but does not ignore '\n'


    // divide
    int data_len = strlen(data);
    for (int i=0; i < data_len; i++)
    {
        if (data[i] == '1') 
        {
            for (int j = 0; j < divisor_len; j++) 
            {
                data[i + j] = xor(data[i + j], divisor[j]);
            }
        }
    }

    // 7 bit remainder
    strncpy(remainder, &data[data_len], divisor_len - 1);
    remainder[divisor_len - 1] = '\0';
    
    for (int i=0; i<divisor_len; i++)
    {
        if (remainder[i] == '1')
        {
            printf("Message is incorrect.\n");
            return;
        }
    }
    printf("Message is correct.\n");

}

int main()
{
    char data[60], d;
    char divisor[10];
    printf("Data: ");
    int i=0;
    for (i=0; d!='\n'; i++)
    {
        scanf("%c", &d);
        data[i] = d;
    }
    data[i-1] = '\0';

    // printf("data is %s", data);

    for(int i = 7; i >= 0; i--) 
    {
        printf("Enter the bit for x^%d: ", i);
        scanf(" %c", &divisor[7 - i]);
    }
    divisor[8] = '\0';

    // clear buffer 
    getchar();

    // printf("divisor is : %s", divisor);


    char* crc = crc_generator(data, divisor);
    printf("crc: %s\n", crc);


    char received_data[60];
    printf("Enter Transmitted data: ");
    d='\0';
    for (i=0; d!='\n'; i++)
    {
        scanf("%c", &d);
        received_data[i] = d;
    }
    received_data[i-1] = '\0';
    crc_checker(received_data, divisor);

    return 0;
}