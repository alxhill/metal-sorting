#include <metal_stdlib>
using namespace metal;

kernel void copy_buffer(constant unsigned int* input, device unsigned int* output, uint index [[thread_position_in_grid]])
{
    output[index] = input[index];
}

kernel void slow_sort(device unsigned int* data, uint index [[thread_position_in_grid]])
{
    uint idx = index*2;
    uint left = data[idx];
    uint right = data[idx+1];

    if (left < right) {
        data[idx] = left;
        data[idx+1] = right;
    } else {
        data[idx] = right;
        data[idx+1] = left;
    }
}

// kernel void slow_sort_copy(device unsigned int* input, device unsigned int* output, uint index [[thread_position_in_grid]])
// {
//     uint idx = index*2;
//     uint left = input[idx];
//     uint right = input[idx+1];

//     if (left < right) {
//         output[idx] = left;
//         output[idx+1] = right;
//     } else {
//         output[idx] = right;
//         output[idx+1] = left;
//     }
// }