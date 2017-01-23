__kernel void moments(__global uchar8* data, 
                      __global float* group_result, 
                      int NUM_WORK_ITEMS, 
                      __local float* local_result, 
                      __global double* central_moments,
                      __global int* workgroups_left,
                      float x_,
                      float y_) {

    const int KERNEL_SIZE = 8;  //for better parallelism it could be enchanced to 16 along with using float16 as input data.
    const int row = get_group_id(0);// / (NUM_WORK_ITEMS); //in range: 0 - NUM_WORK_GROUPS
    const int initial_col = get_local_id(0) * KERNEL_SIZE;
    //by 'moment' we mean central moment
    const int data_offset = get_global_id(0);
    uchar8 input_data = data[data_offset];
    
    //compute central moment MX0 in row
    float momentX0 = input_data.s0 + 
               input_data.s1 + 
               input_data.s2 +
               input_data.s3 +
               input_data.s4 +
               input_data.s5 +
               input_data.s6 +
               input_data.s7;       

    //compute central moment MX1 in a row
    float momentX1 = input_data.s0 * (1 + initial_col - y_) + 
               input_data.s1 * (2 + initial_col - y_) + 
               input_data.s2 * (3 + initial_col - y_) +
               input_data.s3 * (4 + initial_col - y_) +
               input_data.s4 * (5 + initial_col - y_) +
               input_data.s5 * (6 + initial_col - y_) +
               input_data.s6 * (7 + initial_col - y_) +
               input_data.s7 * (8 + initial_col - y_);
               
    //compute central moment MX2 in a row
    float momentX2 = input_data.s0 * (1 + initial_col - y_) * (1 + initial_col - y_) + 
               input_data.s1 * (2 + initial_col - y_) * (2 + initial_col - y_) + 
               input_data.s2 * (3 + initial_col - y_) * (3 + initial_col - y_) +
               input_data.s3 * (4 + initial_col - y_) * (4 + initial_col - y_) +
               input_data.s4 * (5 + initial_col - y_) * (5 + initial_col - y_) +
               input_data.s5 * (6 + initial_col - y_) * (6 + initial_col - y_) +
               input_data.s6 * (7 + initial_col - y_) * (7 + initial_col - y_) +
               input_data.s7 * (8 + initial_col - y_) * (8 + initial_col - y_);
               
    //compute central moment MX3 in a row
    float momentX3 = input_data.s0 * (1 + initial_col - y_) * (1 + initial_col - y_)  * (1 + initial_col - y_) + 
               input_data.s1 * (2 + initial_col - y_) * (2 + initial_col - y_)  * (2 + initial_col - y_) + 
               input_data.s2 * (3 + initial_col - y_) * (3 + initial_col - y_)  * (3 + initial_col - y_) +
               input_data.s3 * (4 + initial_col - y_) * (4 + initial_col - y_)  * (4 + initial_col - y_) +
               input_data.s4 * (5 + initial_col - y_) * (5 + initial_col - y_)  * (5 + initial_col - y_) +
               input_data.s5 * (6 + initial_col - y_) * (6 + initial_col - y_)  * (6 + initial_col - y_) +
               input_data.s6 * (7 + initial_col - y_) * (7 + initial_col - y_)  * (7 + initial_col - y_) +
               input_data.s7 * (8 + initial_col - y_) * (8 + initial_col - y_)  * (8 + initial_col - y_);
       //~ printf("Moment mx2 = %f will be saved to %d\n", momentX2, get_local_id(0) + get_local_size(0) * 2);           
               
   local_result[get_local_id(0) + get_local_size(0) * 0] = momentX0;
   local_result[get_local_id(0) + get_local_size(0) * 1] = momentX1;
   local_result[get_local_id(0) + get_local_size(0) * 2] = momentX2;
   local_result[get_local_id(0) + get_local_size(0) * 3] = momentX3;
   
   barrier(CLK_LOCAL_MEM_FENCE);
   float local_sum = 0;
   float local_MX0 = 0,
         local_MX1 = 0,
         local_MX2 = 0,
         local_MX3 = 0;
   
   if(get_local_id(0) == 0) 
   {
      for(int i=0; i < get_local_size(0); i++) 
      {
         local_MX0 += local_result[i + get_local_size(0) * 0];
         local_MX1 += local_result[i + get_local_size(0) * 1];
         local_MX2 += local_result[i + get_local_size(0) * 2];
         local_MX3 += local_result[i + get_local_size(0) * 3];
      }
      //~ printf("Moment mx2 from all work items inside group = %f will be saved to %d\n", local_MX2, 2 + 4*row);
      group_result[0 + 4*row] = local_MX0;//4 values for each row
      group_result[1 + 4*row] = local_MX1;
      group_result[2 + 4*row] = local_MX2;
      group_result[3 + 4*row] = local_MX3;
      
      //atomic decrement number of workgroups left
      atomic_dec(workgroups_left);
      
      if(0 == *workgroups_left)
      {
          const int number_of_workgroups = get_global_size(0)/get_local_size(0);
          const int COMPUTED_MOMENTS = 4;
          float moment11 = 0.0, moment12 = 0.0, moment30 = 0, moment03 = 0, moment02 = 0,moment20 = 0, moment21 = 0;
          
          //~ printf("Workgroup id:%d. Computing moments hence all local computations finished.\n", row);
          /* group result: 0 + row = MX0
           *               1 + row = MX1
           *               2 + row = MX2
           *               3 + row = MX3
           */
          for(int i=0; i < number_of_workgroups; i++) //change it to number of rows
          {
              //~ printf("Index: %d\n", 2 + i*COMPUTED_MOMENTS);
              //~ printf("Adding to m02 %f\n", group_result[2 + i*COMPUTED_MOMENTS]);
             moment02 += group_result[2 + i*COMPUTED_MOMENTS];
             moment03 += group_result[3 + i*COMPUTED_MOMENTS];
             moment11 += (i+1 - x_) * group_result[1 + i*COMPUTED_MOMENTS];
             moment12 += (i+1 - x_) * group_result[2 + i*COMPUTED_MOMENTS];
             moment21 += (i+1 - x_) *(i+1 - x_) * group_result[1 + i*COMPUTED_MOMENTS];
             moment20 += (i+1 - x_) *(i+1 - x_) * group_result[0 + i*COMPUTED_MOMENTS];
             moment30 += (i+1 - x_) *(i+1 - x_) * (i+1 - x_) * group_result[0 + i*COMPUTED_MOMENTS];
          }
          //messed up the moments to match those computed in opencv
          central_moments[0] = moment20;
          central_moments[1] = moment30;
          central_moments[2] = moment11;
          central_moments[3] = moment21;
          central_moments[4] = moment02;
          central_moments[5] = moment12;
          central_moments[6] = moment03;
      }
      
      //~ printf("Number of workgroups left: %d\n", *workgroups_left);
      //~ printf("Row: %d, Computed sum inside: %f\n", row, group_result[row]);
   }
}
