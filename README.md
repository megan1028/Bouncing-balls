# Bouncing-balls
OpenCL

• Starting with the control flow of the 2D Bouncing Balls simulation, divide the work to run on the CPU (which is good for task parallelism)
and on the GPU (which is good for data parallelism). The goal of your work division should be to minimize data transfer between the CPU’s host memory and the GPU’s
device memory as well as to minimize (completely avoid, if possible) any synchronization.
• Develop the kernel code for the CPU and the kernel code for the GPU according to their designs.
• Develop the necessary host program to run on the CPU which will control and coordinate the two kernel programs running on the CPU and GPU. Also implement the
necessary OpenGL calls from OpenCL to display the image frame on the screen.
