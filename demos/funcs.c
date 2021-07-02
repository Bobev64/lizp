#include<stdio.h>

int add_together(int x, int y);

int main(void) {

    int output = add_together(3,5);
    printf("%d\n", output);

    return 0;
}

int add_together(int x, int y) {
    return x + y;
}
