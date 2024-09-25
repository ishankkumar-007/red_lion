#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define BYTE 8

char vrc(char sent_data[])
{
    // char parity = sent_data[0];
    // for(int i=1;i<BYTE;i++)
    // {
    //     parity ^= sent_data[i];
    // }

    // return parity;

    int count = 0;
    for (int i = 0; i < BYTE; i++) {
        if (sent_data[i] == '1') {
            count++;
        }
    }
    // Return 0 if even parity, else return 1 for odd parity
    return (count % 2 == 0) ? '0' : '1';     
}

int check_error(char sent_data[],char receiced_data[])
{
    char sent_parity = vrc(sent_data);

    printf("Parity bit sent --- %c\n",sent_parity);

    char received_parity = receiced_data[0];

    if(sent_parity!=received_parity)
    return 1;

    return 0;
}

int main()
{
    char sent_data[BYTE+1];
    char received_data[BYTE+2];

    printf("Enter the sent data (8 bits) : ");
    scanf("%s",sent_data);

    printf("Enter the received data (9 bits) : ");
    scanf("%s",received_data);

    int error = check_error(sent_data,received_data);

    if(!error)
    {
        printf("No error Found!\n");
        return 0;
    }

    printf("Error Found\n");

    return 0;
}