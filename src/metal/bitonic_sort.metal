#include <metal_stdlib>
using namespace metal;

kernel void bitonic_swap_asc(device unsigned int* left, device unsigned int* right, uint index [[thread_position_in_grid]])
{
    unsigned int l = left[index];
    unsigned int r = right[index];
    left[index] = min(l, r);
    right[index] = max(l, r);
}

kernel void bitonic_swap_dec(device unsigned int* left, device unsigned int* right, uint index [[thread_position_in_grid]])
{
    unsigned int l = left[index];
    unsigned int r = right[index];
    left[index] = max(l, r);
    right[index] = min(l, r);
}
