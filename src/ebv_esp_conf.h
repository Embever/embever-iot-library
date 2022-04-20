#ifndef INC_EBV_ESP_CONF
#define INC_EBV_ESP_CONF

#define ESP_DELAYED_RESPONSE_HEADER_LEN   4
#define EBV_TMP_BUFF_MAXSIZE            255
#define EBV_ESP_MAX_PAYLOAD_SIZE        512

#define ESP_CMD_LEN                       1
#define ESP_CRC_LEN                       4
#define ESP_FLAGS_LEN                     1
#define ESP_PACKET_OVERHEAD (ESP_CMD_LEN + ESP_CRC_LEN + ESP_FLAGS_LEN)

#endif