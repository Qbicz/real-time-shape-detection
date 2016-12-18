__kernel void moments(__global float8* data, 
                      __global float* group_result, 
                      int NUM_WORK_ITEMS, 
                      __local float* local_result, 
                      __global float* moments) {

    const int KERNEL_SIZE = 8;  //for better parallelism it could be enchanced to 16 along with using float16 as input data.
    const int row = get_global_id(0) / (NUM_WORK_ITEMS); //in range: 0 - NUM_WORK_GROUPS
    const int initial_col = get_local_id(0) * KERNEL_SIZE;
    float moment11 = 0, local_sum = 0;
    const int data_offset = get_global_id(0);
    float8 input_data = data[data_offset];
    
    //compute moment M11
    moment11 =  input_data.s0 * (1 + initial_col) + 
                input_data.s1 * (2 + initial_col) + 
                input_data.s2 * (3 + initial_col) +
                input_data.s3 * (4 + initial_col) +
                input_data.s4 * (5 + initial_col) +
                input_data.s5 * (6 + initial_col) +
                input_data.s6 * (7 + initial_col) +
                input_data.s7 * (8 + initial_col);
                
   local_result[get_local_id(0)] = moment11;
   barrier(CLK_LOCAL_MEM_FENCE);

   if(get_local_id(0) == 0) 
   {
      for(int i=0; i<get_local_size(0); i++) 
      {
         local_sum += local_result[i];
      }
      group_result[row] = local_sum;
      printf("Row: %d, Computed sum inside: %f\n", row, group_result[row]);
   }

    //~ barrier(CLK_LOCAL_MEM_FENCE);
    barrier(CLK_GLOBAL_MEM_FENCE); //is it neccessary?
    
   if(get_global_id(0) == 0) 
   {
      for(int m = 0; m < 3; m++) //change '3' to the number of moments to be computed (hardcoded)
      {
          float moment11 = 0.0, moment10 = 0.0;;
          for(int i=0; i<4; i++) //change it to number of rows
          {
             moment10 += group_result[i]; 
             moment11 += (i+1) * group_result[i];
          }
          moments[0] = moment10;
          moments[1] = moment11;
      }
   }
}
