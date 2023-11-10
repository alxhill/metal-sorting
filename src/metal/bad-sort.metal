#include <metal_stdlib>
using namespace metal;

kernel void double_value(device const unsigned int* input, device unsigned int* output, uint index [[thread_position_in_grid]])
{
    output[index] = input[index] * 2;
}