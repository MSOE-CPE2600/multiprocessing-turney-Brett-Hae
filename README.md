# System Programming Lab 11 Multiprocessing
Multiprocessing implementation is similar to our ICA9 12 core implementation.
It will loop for the 50 desired frames creating a new child when there aren't the designated amount running.
Each child will calculate its name and scale amount then call the mandel executable with those parameters inputted.
The results show that runtime halves until it processes start colliding with other processes on the machine.
When they start colliding, there is minimal time saving