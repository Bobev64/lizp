#include<stdio.h>

int spam(int n);

int main(void) {
    
    spam(5);

    return 0;
}

int spam(int n) {
    while (n > 0) {
        return printf("spam\n") && spam(n-1);
    }
}
