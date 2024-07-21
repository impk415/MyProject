Hello.

To run the code, firstly you should open the input file named "inp.txt". 
You should first enter the value of N(i.e the no. of rows in the matrix), then give a space, then enter the value of K(i.e the no. of threads), then a space, then enter the value of rowInc.

Then on the next line, please give the input matrix as SPACE-SEPERATED INTEGERS with each row on a new line.

For example, suppose N=4, K=2, and the input matrix can be given as an input in the "inp.txt" as follows:

4 2
1 3 5 2
4 9 6 1
0 7 9 5
6 4 2 6

Now after giving the input, IF YOU HAVE VS CODE on linux OS, you simply need to run the code. 
Corresponding to the code which uses dynamic_TAS technique to distribute load among the threads, an output file named "output_TAS.txt" will be created.
(i.e "Assign3Src_ES22BTECH11026_TAS.cpp")

Corresponding to the code which uses dynamic_CAS technique to distribute load among the threads, an output file named "output_CAS.txt" will be created.
(i.e "Assign3Src_ES22BTECH11026_CAS.cpp")

Corresponding to the code which uses dynamic_TAS technique to distribute load among the threads, an output file named "output_Bounded_CAS.txt" will be created.
(i.e "Assign3Src_ES22BTECH11026_Bounded_CAS.cpp")

Corresponding to the code which uses dynamic_CAS technique to distribute load among the threads, an output file named "output_AtomicIncrement.txt" will be created.
(i.e "Assign3Src_ES22BTECH11026_Atomic_Increment.cpp")

