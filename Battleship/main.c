#include <stdio.h>
#include <string.h>

int main() {
    unsigned char name = 'A';
    unsigned char data[25] = "cndwnclwqnciu";
    unsigned char crc[2] = "fg";
    unsigned char string[30];

    // Use %c for individual characters in data and crc
    sprintf(string, "%c:", name);
    
    // Append each character in data to the string
    for (int i = 0; i < 25; i++) {
        sprintf(string + strlen(string), "%c", data[i]);
    }

    // Append a comma
    sprintf(string + strlen(string), ",");

    // Append each character in crc to the string
    for (int i = 0; i < 2; i++) {
        sprintf(string + strlen(string), "%c", crc[i]);
    }

    printf("%s, %d\n", string, strlen(string));

    return 0;
}
