#include <metal_stdlib>
using namespace metal;

kernel void slow_sort(device unsigned int* data, uint index [[thread_position_in_grid]])
{
    uint idx = index*2;
    uint left = data[idx];
    uint right = data[idx+1];

    data[idx] = min(left, right);
    data[idx+1] = max(left, right);
}