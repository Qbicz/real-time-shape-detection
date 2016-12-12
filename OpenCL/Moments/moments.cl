__kernel void moments(__global float8* data, __global float* group_result) {


   //float4 input1, input2, sum_vector;
   
     /*
     * Each kernel calculates 16 output pixels in the same data_ptr (hence the '* 16').
     * column is in the range [0, width] in steps of 16.
     * data_ptr is in the range [0, height].
     */
    //const width = 512;
    //const int column = get_global_id(0) * 16;
    
    
    
    
    const int KERNEL_SIZE = 8;
    const int data_ptr = get_local_id(0) * KERNEL_SIZE;
    const int row = get_local_id(0);
    float8 sum = 0;
    float8 input_data = data[data_ptr];
    
    
    
    
    
    
    //float8 input_data;
    
    //~ for(int j = 0; j < ROW_WIDTH; j++)
    //~ {
        //~ if(get_local_id(0) == 0)
            //~ printf("Adding %f\n", data[data_ptr+j]);
        //~ sum += data[data_ptr+j] * j;
    //~ }
    
    //~ group_result[get_local_id(0)] = sum;
    printf("Executing kernel %d with data_ptr %d which starts with %5.2f \n", get_local_id(0), data_ptr, data[data_ptr]);
    /* Offset calculates the position in the linear data for the data_ptr and the column. */
    //const int offset = data_ptr * width + column;
    
    //size_t globalWorksize[2] = {width / 16, height / 1};
   
   
   
   //~ uint global_addr, local_addr;
   //global_addr = get_global_id(0) * 2;
   //input1 = data[global_addr];
   //input2 = data[global_addr+1];
   //sum_vector = input1 + input2;

   //~ local_addr = get_local_id(0);
   //~ local_result[local_addr] = sum_vector.s0 + sum_vector.s1 + 
                              //~ sum_vector.s2 + sum_vector.s3; 
                    
   //~ local_result[local_addr] = data_ptr.s0 + data_ptr.s1 + data_ptr.s2 + data_ptr.s3 + data_ptr.s4 + data_ptr.s5 + data_ptr.s6 + data_ptr.s7; 
                              
                              
   //~ barrier(CLK_LOCAL_MEM_FENCE);

   //~ if(get_local_id(0) == 0) {
      //~ sum = 0.0f;
      //~ for(int i=0; i<get_local_size(0); i++) {
         //~ sum += local_result[i];
      //~ }
      //~ group_result[get_group_id(0)] = sum;
   //~ }
}
