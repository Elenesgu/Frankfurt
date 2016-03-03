__kernel void FRAInvertImage(__global unsigned char* bits)
{
size_t id = get_global_id(0);
bits[id] = 255 - bits[id];
}