#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for exit()
#include <sys/wait.h>
#include <stdbool.h>

#define INTSIZE sizeof(int)

int isPrime(int number) {
    if(number < 2) {
        return false;
    }

    for (int i = 2; i * i <= number; ++i) {
        if (number % i == 0) {
            return false;
        }
    }

    return true;
}

void findPrimes(int child_num, int start, int stop, int p[]) {
    for(int i = start; i <= stop; ++i) {
        if(isPrime(i)) {
            write(p[1], &i, INTSIZE);
        }
    }
    
}

int main()
{
    int p[2], i;
    const int NUM_P = 10;

    if (pipe(p) < 0)
        exit(1);

    for(i = 0; i < NUM_P; ++i) {
        pid_t pid = fork();

        if(pid == 0) {
            findPrimes(i + 1, 1000 * i + 1, 1000 * (i + 1), p);
            _exit(0);
        } else if (pid < 0) {
            perror("fork");
            return 1;
        }

    }

    close(p[1]);
    int received_data;
    while(read(p[0], &received_data, INTSIZE) > 0) {
        printf("%d\n", received_data);
    }

    close(p[0]);
    for(i =0; i < NUM_P; i++) {
        wait(NULL);
    }
    
    return 0;
}
