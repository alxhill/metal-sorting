#include <metal_stdlib>
using namespace metal;

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
    // data[idx] = min(left, right);
    // data[idx+1] = max(left, right);
}

kernel void slow_sort_copy(const unsigned int* input, device unsigned int* output, uint index [[thread_position_in_grid]])
{
    uint idx = index*2;
    uint left = input[idx];
    uint right = input[idx+1];

    if (left < right) {
        output[idx] = left;
        output[idx+1] = right;
    } else {
        output[idx] = right;
        output[idx+1] = left;
    }
}