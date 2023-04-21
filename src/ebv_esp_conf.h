#ifndef INC_EBV_ESP_CONF
#define INC_EBV_ESP_CONF

#define ESP_DELAYED_RESPONSE_HEADER_LEN   4
#define EBV_TMP_BUFF_MAXSIZE            255
#define EBV_ESP_MAX_PAYLOAD_SIZE        512

#define ESP_CMD_SIZE                            1
#define ESP_RESPONSE_ID_SIZE                    1
#define ESP_DELAYED_RESP_ID_SIZE                1
#define ESP_PKG_LEN_SIZE                        2
#define ESP_CMD_HEADER_SIZE                     (ESP_CMD_SIZE + ESP_PKG_LEN_SIZE)
#define ESP_DELAYED_RESPONSE_HEADER_SIZE        (ESP_RESPONSE_ID_SIZE + ESP_DELAYED_RESP_ID_SIZE + ESP_PKG_LEN_SIZE)
#define ESP_CRC_LEN                             4
#define ESP_FLAGS_LEN                           1
#define ESP_CMD_PACKET_OVERHEAD                 (ESP_CMD_HEADER_SIZE + ESP_FLAGS_LEN + ESP_CRC_LEN)
#define ESP_DELAYED_RESPONSE_PAYLOAD_OVERHEAD   (ESP_RESPONSE_ID_SIZE + ESP_CMD_HEADER_SIZE + ESP_CRC_LEN + ESP_FLAGS_LEN)  // Fixed data length of the delayed response payload overhead
#define ESP_DELAYED_RESPONSE_OVERHEAD           (ESP_DELAYED_RESPONSE_HEADER_SIZE + ESP_DELAYED_RESPONSE_PAYLOAD_OVERHEAD)  // Fixed data length of the delayed response overhead, all the length instead of the payload 

#endif