__kernel void dataParallel(__global float* radius, __global float* randomNum, __global float* x)
 {
     int num = get_global_id(0);
     x[num] = randomNum[4*num] - (1.0 - *radius); //  random number between	
		
 }
 __kernel void dataParallel2(__global float* radius, __global float* randomNum, __global float* y)
 {
     int num = get_global_id(0);
     y[num] = randomNum[4*num + 1] - (1.0 - *radius); //  random number between	
		
 }

 __kernel void dataParallel3(__global float* radius, __global float* randomNum, __global float* vx)
 {
     int num = get_global_id(0);
     vx[num] = randomNum[4*num + 2] - 0.01; //  random number between	
		
 }

 __kernel void dataParallel4(__global float* radius, __global float* randomNum, __global float* vy)
 {
     int num = get_global_id(0);
     vy[num] = randomNum[4*num + 3] - 0.01; //  random number between	
		
 }






