#include "battleship_crc.h"


uint16_t calculateCrc(const uint8_t *data) {
    uint16_t crc = 0xFFFF;  // Initial CRC value, set to 0xFFFF as per CRC-CCITT.

    for (uint8_t i = 0; i < MSG_SIZE - CRC_SIZE; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ POLYNOMIAL;
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc & 0xFFFF;  // Mask the result to 16 bits.
}

void appendCrcCode(uint8_t *data){
    CrcCode crcCode;
    crcCode.crc = calculateCrc((const uint8_t*) data);
    data[MSG_SIZE - CRC_SIZE + 1] = crcCode.crcBy8[1];
    data[MSG_SIZE - CRC_SIZE] = crcCode.crcBy8[0];
}

bool checkCrc(uint8_t *data){
    static CrcCode crcCode;
    crcCode.crc = calculateCrc((const uint8_t*) data);
    return (crcCode.crcBy8[1] == data[MSG_SIZE - CRC_SIZE + 1]) && (crcCode.crcBy8[0] == data[MSG_SIZE - CRC_SIZE]);
}

void eraseCrcCode(uint8_t data[]){
	for(uint8_t i = MSG_SIZE - CRC_SIZE; i < MSG_SIZE; i++){
		data[i] = '\0';
	}
}