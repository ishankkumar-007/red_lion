#include <stdio.h>
#include <string.h>

#define BYTE_SIZE 8
#define NUM_BYTES 4  

void calculate_checksum(char data[NUM_BYTES][BYTE_SIZE + 1], char checksum[BYTE_SIZE + 1]) {
    int carry = 0;
    int sum[BYTE_SIZE] = {0};

    for (int i = 0; i < BYTE_SIZE; i++) {
        sum[i] = (data[0][BYTE_SIZE - 1 - i] - '0') + carry;
        for (int j = 1; j < NUM_BYTES; j++) {
            sum[i] += (data[j][BYTE_SIZE - 1 - i] - '0');
        }
        carry = sum[i] / 2;
        sum[i] %= 2;
    }


    while (carry > 0) {

        for (int i = 0; i < BYTE_SIZE; i++) {
            sum[i] += carry;
            carry = sum[i] / 2;
            sum[i] %= 2;
            if (carry == 0) break;
        }
    }

    // Calculate 1's complement of the sum for the checksum
    for (int i = 0; i < BYTE_SIZE; i++) {
        checksum[i] = (sum[BYTE_SIZE - 1 - i] == 1) ? '0' : '1';  // 1's complement
    }
    
}

void check_data_for_errors(char received_data[NUM_BYTES][BYTE_SIZE + 1], char sent_checksum[BYTE_SIZE + 1]) {
    int carry = 0;
    int sum[BYTE_SIZE] = {0};

    // Perform binary addition between the received data and the sent checksum
    for (int i = 0; i < BYTE_SIZE; i++) {
        
        sum[i] = (sent_checksum[BYTE_SIZE - 1 - i] - '0') + carry;

        for (int j = 0; j < NUM_BYTES; j++) {
            sum[i] += (received_data[j][BYTE_SIZE - 1 - i] - '0');
        }


        carry = sum[i] / 2;
        sum[i] %= 2;
    }

    
    while (carry > 0) {
        for (int i = 0; i < BYTE_SIZE; i++) {
            sum[i] += carry;
            carry = sum[i] / 2;
            sum[i] %= 2;
            if (carry == 0) break;
        }
    }

    
    int all_ones = 1;
    for (int i = 0; i < BYTE_SIZE; i++) {
        if (sum[i] != 1) {
            all_ones = 0;
            break;
        }
    }

    if (all_ones) {
        printf("No error detected (Checksum gives all 1's).\n");
    } else {
        printf("Error detected (Checksum does not give all 1's).\n");
    }
}


int main() {
    char sent_data[NUM_BYTES][BYTE_SIZE + 1];    
    char sent_checksum[BYTE_SIZE + 1]; 
    char received_data[NUM_BYTES][BYTE_SIZE + 1]; 
    char received_checksum[BYTE_SIZE + 1]; 

    printf("Sender Side: Enter the sent data (4 bytes of 8 bits each):\n");
    for (int i = 0; i < NUM_BYTES; i++) {
        printf("Enter 8-bit data for byte %d: ", i + 1);
        scanf("%s", sent_data[i]);
    }


    calculate_checksum(sent_data, sent_checksum);
    printf("Calculated Checksum (Sender Side): %s\n", sent_checksum);


    printf("\nReceiver Side: Enter the received data (4 bytes of 8 bits each):\n");
    for (int i = 0; i < NUM_BYTES; i++) {
        printf("Enter 8-bit data for byte %d: ", i + 1);
        scanf("%s", received_data[i]);
    }


    check_data_for_errors(received_data, sent_checksum);

    return 0;
}
