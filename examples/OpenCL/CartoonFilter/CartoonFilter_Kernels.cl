/**********************************************************************
Copyright ©2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#define DATA_TYPE uchar
#define MAX_NUM 255
#define CV_DESCALE(x,n) (((x) + (1 << ((n)-1))) >> (n))

enum
{
    yuv_shift  = 14,
    R2Y        = 4899,
    G2Y        = 9617,
    B2Y        = 1868,
};

__kernel void RGB2Gray(int cols,int rows,int src_step,int dst_step,int channels,
                       int bidx, __global const DATA_TYPE* src, __global DATA_TYPE* dst)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    src_step /= sizeof(DATA_TYPE);
    dst_step /= sizeof(DATA_TYPE);
    if (y < rows && x < cols)
    {
        int src_idx = y * src_step + x * channels;
        int dst_idx = y * dst_step + x;
        dst[dst_idx] = (DATA_TYPE)CV_DESCALE((src[src_idx + bidx] * B2Y + src[src_idx + 1] * G2Y + src[src_idx + (bidx^2)] * R2Y), yuv_shift);
    }
}
__kernel void Gray2RGB(int cols,int rows,int src_step,int dst_step,
                       __global const DATA_TYPE* src, __global DATA_TYPE* dst)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    src_step /= sizeof(DATA_TYPE);
    dst_step /= sizeof(DATA_TYPE);
    if (y < rows && x < cols)
    {
        int src_idx = y * src_step + x;
        int dst_idx = y * dst_step + x * 4;
        DATA_TYPE val = src[src_idx];
        dst[dst_idx++] = val;
        dst[dst_idx++] = val;
        dst[dst_idx++] = val;
        dst[dst_idx] = MAX_NUM;
    }
}