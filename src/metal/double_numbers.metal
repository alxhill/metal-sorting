#include <metal_stdlib>
using namespace metal;

kernel void double_value(device unsigned int* data, uint index [[thread_position_in_grid]])
{
    data[index] = data[index] * 2;
}

kernel void double_value_pair(device unsigned int* data, uint index [[thread_position_in_grid]])
{
    data[index*2] = data[index*2] * 2;
    data[index*2+1] = data[index*2+1] * 2;
}

kernel void double_value_eight(device unsigned int* data, uint index [[thread_position_in_grid]]) {
    for (int i = 0; i < 8; i++) {
        data[index*8+i] = data[index*8+i] * 2;
    }
}

kernel void double_value_64(device unsigned int* data, uint index [[thread_position_in_grid]]) {
    for (int i = 0; i < 64; i++) {
        data[index*64+i] = data[index*64+i] * 2;
    }
}

kernel void double_value_128(device unsigned int* data, uint index [[thread_position_in_grid]]) {
    for (int i = 0; i < 128; i++) {
        data[index*128+i] = data[index*128+i] * 2;
    }
}
