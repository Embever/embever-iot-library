#include "ebv_i2c.h"

#include <string.h>

#define MAX_I2C_BUFFER_LEN  64
#define I2C_BUFFERS         10

struct i2c_buffer_s{
    unsigned char buffer[MAX_I2C_BUFFER_LEN];
    int buffer_len;
    int index;
};

static struct i2c_buffer_s i2c_buffer[I2C_BUFFERS];
static int i2c_buffer_index_in;
static int i2c_buffer_index_out;

void wire_begin(uint8_t address);
void wire_end();
int wire_available();
size_t wire_write(uint8_t data);
size_t wire_requestFrom(uint8_t address, uint16_t nof_bytes);
int wire_read();

// register i2c functions into ebv_iot_lib
void mock_i2c_init(){
    i2c_buffer_index_out = -1;              // Set to an invalid address
    i2c_buffer_index_in = 0;
    memset(i2c_buffer, 0, sizeof(i2c_buffer));

    // register functions
    struct ebv_i2c_cb cb;             
    cb.beginXfer = wire_begin;        
    cb.stopXfer = wire_end;           
    cb.dataAvailable = wire_available;
    cb.dataWrite = wire_write;        
    cb.dataRequest = wire_requestFrom; 
    cb.dataRead = wire_read;          
    ebv_i2c_registerI2c(&cb);         
}

void wire_begin(uint8_t address){

}

void wire_end(){
    
}

int wire_available(){
    if(i2c_buffer_index_out < 0){
        return 0;
    } else {
        return i2c_buffer[i2c_buffer_index_out].buffer_len;
    }
}

size_t wire_write(uint8_t data){

}

size_t wire_requestFrom(uint8_t address, uint16_t nof_bytes){
    // set the right index buffer
    if(i2c_buffer_index_out < (I2C_BUFFERS - 1)){
        i2c_buffer_index_out++;
    }
    i2c_buffer[i2c_buffer_index_out].index = 0;
    return i2c_buffer[i2c_buffer_index_out].buffer_len;
}

int wire_read(){
    if(i2c_buffer_index_out >= 0){
        unsigned char data = i2c_buffer[i2c_buffer_index_out].buffer[i2c_buffer[i2c_buffer_index_out].index];
        i2c_buffer[i2c_buffer_index_out].index++;
        if(i2c_buffer[i2c_buffer_index_out].index >= MAX_I2C_BUFFER_LEN){
            i2c_buffer[i2c_buffer_index_out].index = MAX_I2C_BUFFER_LEN -1;
        }
        return data;
    } else {
        return 0;
    }
}

// mock functions
bool mock_ebv_i2c_set_response(const unsigned char *data, int data_len){
    if(i2c_buffer_index_in >= I2C_BUFFERS || data_len > MAX_I2C_BUFFER_LEN){
        return false;
    }

    memcpy(i2c_buffer[i2c_buffer_index_in].buffer, data, data_len);
    i2c_buffer[i2c_buffer_index_in].buffer_len = data_len;
    i2c_buffer_index_in++;
    return true;
}

bool mock_ebv_i2c_set_delayed_response(const unsigned char *data, int data_len){
    if((i2c_buffer_index_in + 1) >= I2C_BUFFERS || data_len > MAX_I2C_BUFFER_LEN){
        return false;
    }
    // Need to put it in twice due to ESP delayed response mechanism
    memcpy(i2c_buffer[i2c_buffer_index_in].buffer, data, data_len);
    i2c_buffer[i2c_buffer_index_in].buffer_len = data_len;
    i2c_buffer_index_in++;

    memcpy(i2c_buffer[i2c_buffer_index_in].buffer, data, data_len);
    i2c_buffer[i2c_buffer_index_in].buffer_len = data_len;
    i2c_buffer_index_in++;
    return true;
}
