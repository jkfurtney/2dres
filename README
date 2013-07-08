To run the reservoir simulator:


*******************************************************************
                             First step
*******************************************************************

Erase all the .o files that may exist:
>rm *.o

Compile the code:
> make

Run the executable file
>./exec

At the beginning, you have to choose various parameters.
By typing the key 'return', you choose the values by default.

The program creates several files:
1) coorp.mat that gives the coordinates of each point of the mesh
2) coort.mat that gives for each triangle of the mesh its 3 points 
3) result.mat that take for each saved time the pressure and the
saturation at each point
4) frac.mat that gives the fraction of the area of the reservoir
flooded by the injected fluid (alpha=1)

In addition, the program saves at each time step in alpha_out.mat
the state of the saturation in case you want to restart the 
simulation from this last value of alpha.

to visualise the data, I launch Matlab in the directory where these
files have been created and type the command
>>view_matlab

that run the matlab file view_matlab.m

This is not necessary to have Matlab to visualise the result.
The data is saved is the *.mat text files. 


*******************************************************************
                           Second step
*******************************************************************

If I want to change the parameters during the simulation,

- I stop the computation, 
>control C

- I move the last saturation state saved into the file alpha_in.out
>mv alpha_out.mat alpha_in.mat

- I restart the simulation 
>./exec

- I change the parameters and answer 1 
at the question: Load an inital alpha?





*******************************************************************
                           Third step
*******************************************************************

If you want to switch on/off producer, create or move leaks,
you can 
- either change the mesh file,
- or change values in the file Mesh.cc at the function readMesh


The format of the file that contains the definition of the mesh
used is the .amdba consisting of:
1) a list a points defined by their coordinates and a reference
2) a list of triangle defined by 3 points and a reference

Example: 'test.amdba'
We have 5 points that define 4 triangles.
The points may have the reference 0, 3 or 4 and
the triangle have all the reference 0.

the file 'test.amdba' is a test file with:
5 4 -- nbs,nbt 
         1   0.0000000E+00   1.0000000E+00        4
         2   1.0000000E+00   1.0000000E+00        4
         3   0.5000000E+00   0.5000000E+00        0
         4   0.0000000E+00   0.0000000E+00        3
         5   1.0000000E+00   0.0000000E+00        3
     1      1      3      2        0 
     2      2      3      5        0 
     3      5      3      4        0 
     4      4      3      1        0 



In my program, I do not use the reference of the triangles.
The point references are defined in the file 'mylibrary.hh':

Example:
#define LEAK        5
#define INJECTOR    4
#define PRODUCER    3
#define BORDER      2
#define OPEN        1
#define NORMAL      0
A point of reference 2 is on a BORDER.


Actually, in the meshes, I have only defined 
INJECTOR, PRODUCER, BORDER and NORMAL. 
I switch on/off an injecting well or create a leak in the
file Mesh.cc in the function meshRead.


