#include <metal_stdlib>
using namespace metal;

kernel void double_value(device const unsigned int* input, device unsigned int* output, uint index [[thread_position_in_grid]])
{
    output[index] = input[index] * 2;
}

// same as above but does 64 elements at a time
kernel void double_value_block(
    device const unsigned int* input,
    device unsigned int* output,
    uint thread_index [[thread_position_in_grid]],
    uint therad_group_id [[threadgroup_position_in_grid]])
{
    uint start_index = therad_group_id * 64 + thread_index;

    for (uint i = start_index; i < start_index + 64; i++)
    {
        output[i] = input[i] * 2;
    }
}