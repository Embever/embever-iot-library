#ifndef INC_MOC_EBV_I2C_H
#define INC_MOC_EBV_I2C_H

#include <stdio.h>

#define _MOCK_I2C_CRC                                    0x00,0x00,0x00,0x00
#define _MOCK_I2C_FLAG                                                  0x01
#define _MOCK_I2C_ESP_ERROR_HEADER                                 0xB3,0xA5
#define _MOCK_I2C_ESP_ERROR_CODE_RESOURCE_BUSY                     0x01,0x08
#define _MOCK_I2C_DELAYED_RESPONSE_HEADER                          0x55,0xA1
#define _MOCK_I2C_READ_LOCAL_FILE_RESPONSE_HEADER                  0x055,0xA7


#define MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE                       {0x55, 0xA7}
#define MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_OK     {   _MOCK_I2C_DELAYED_RESPONSE_HEADER, 0x0C, 0x00,              \
                                                                        _MOCK_I2C_READ_LOCAL_FILE_RESPONSE_HEADER, 0x08, 0x00,      \
                                                                        0x92, 0x00, 0x00,                                           \
                                                                        _MOCK_I2C_CRC,                                              \
                                                                        _MOCK_I2C_FLAG}
#define MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_FAIL   {   _MOCK_I2C_DELAYED_RESPONSE_HEADER, 0x0D, 0x00,                          \
                                                                        _MOCK_I2C_READ_LOCAL_FILE_RESPONSE_HEADER, 0x09, 0x00,                  \
                                                                        _MOCK_I2C_ESP_ERROR_HEADER,_MOCK_I2C_ESP_ERROR_CODE_RESOURCE_BUSY,      \
                                                                        _MOCK_I2C_CRC,_MOCK_I2C_FLAG                                            \
                                                                        }
void mock_i2c_init();
bool mock_ebv_i2c_set_response(int resp_number, const unsigned char *data, int data_len);

#endif