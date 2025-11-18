/**
 * mandelmovie.c
 * Executes the mandel function 50 times with an entered number of processes (default 1)
 * Brett Haensgen
 * 11/18/2025
 * gcc mandelmovie.c -o mandelmovie
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    int frames = 50;
    int max = 1;
    int children = 0;
    float scale = 8;

    // Parse input for number of processors
    int num = atoi(argv[1]);
    if(argc > 1 || num > 0) {
        max = num;
    }

    for(int i=0; i<frames; i++) {
        // If children processes are maxed, wait for one to be done before continuing
        while(children>=max) {
            wait(NULL);
            children--;
        }

        float currentScale = scale - (0.1f * i);

        pid_t pid = fork();
        if(pid == 0) { // Child process
            // Create file name
            char filename[100];
            snprintf(filename, sizeof(filename), "mandel%d.jpg", i);

            // Calculate scalar
            char nextScale[20];
            snprintf(nextScale, sizeof(nextScale), "%.2f", currentScale);

            // Call executable file with desired parameters
            execl("./mandel", "mandel", "-o", filename, "-s", nextScale, NULL);
        } else { // Parent process
            children++;
        }
    }

    // Wait for the rest of the children to be done
    while(children > 0) {
        wait(NULL);
        children--;
    }

    return 0;
}