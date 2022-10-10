#ifndef INC_MOC_EBV_I2C_H
#define INC_MOC_EBV_I2C_H

#include <stdio.h>

#define ESP_CMD_READ_LOCAL_FILE 0xA7

#define _MOCK_I2C_CRC                                    0x00,0x00,0x00,0x00
#define _MOCK_I2C_FLAG                                                  0x01
#define _MOCK_I2C_ESP_ERROR_HEADER                                 0xB3,0xA5
#define _MOCK_I2C_ESP_ERROR_CODE_INVALID_CMD_DATA                  0x01,0x02
#define _MOCK_I2C_ESP_ERROR_CODE_INTERNAL_ERROR                    0x01,0x07
#define _MOCK_I2C_ESP_ERROR_CODE_RESOURCE_BUSY                     0x01,0x08
#define _MOCK_I2C_DELAYED_RESPONSE_HEADER                          0x55,0xA1
#define _MOCK_I2C_READ_LOCAL_FILE_RESPONSE_HEADER                  0x055,ESP_CMD_READ_LOCAL_FILE


#define MOCK_I2C_RESPONSE_ACK_READ_LOCAL_FILE                           {0x55, 0xA7}
#define MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_OPEN_WRITE_OK         {   _MOCK_I2C_DELAYED_RESPONSE_HEADER, 0x0C, 0x00,              \
                                                                            _MOCK_I2C_READ_LOCAL_FILE_RESPONSE_HEADER, 0x08, 0x00,      \
                                                                            0x92, 0x00, 0x00,                                           \
                                                                            _MOCK_I2C_CRC,                                              \
                                                                            _MOCK_I2C_FLAG                                              \
                                                                        }
#define MOCK_I2C_DELAYED_RESPONSE_FAIL(CMD,ERROR_CODE_1,ERROR_CODE_2)   {    _MOCK_I2C_DELAYED_RESPONSE_HEADER, 0x0D, 0x00,            \
                                                                             0x55, CMD, 0x09, 0x00,                                    \
                                                                             ERROR_CODE_1, ERROR_CODE_2, _MOCK_I2C_ESP_ERROR_HEADER,   \
                                                                             _MOCK_I2C_CRC, _MOCK_I2C_FLAG                             \
                                                                        }
#define MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_FAIL(ERR)             MOCK_I2C_DELAYED_RESPONSE_FAIL(ESP_CMD_READ_LOCAL_FILE, ERR)
#define MOCK_I2C_DELAYED_RESPONSE_READ_LOCAL_FILE_WRITE_OK              {   _MOCK_I2C_DELAYED_RESPONSE_HEADER, 0x04, 0x00,              \
                                                                            _MOCK_I2C_READ_LOCAL_FILE_RESPONSE_HEADER, 0x00, 0x00,      \
                                                                        }

void mock_i2c_init();
bool mock_ebv_i2c_set_response(const unsigned char *data, int data_len);
bool mock_ebv_i2c_set_delayed_response(const unsigned char *data, int data_len);

#endif