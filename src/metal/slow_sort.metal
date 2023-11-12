#include <metal_stdlib>
using namespace metal;

kernel void slow_sort_even(device unsigned int* data, uint index [[thread_position_in_grid]])
{
    unsigned int left = data[index*2];
    unsigned int right = data[index*2+1];

    data[index*2] = min(left, right);
    data[index*2+1] = max(left, right);
}

kernel void slow_sort_odd(device unsigned int* data, uint index [[thread_position_in_grid]])
{
    unsigned int left = data[index*2+1];
    unsigned int right = data[index*2+2];

    data[index*2+1] = min(left, right);
    data[index*2+2] = max(left, right);
}