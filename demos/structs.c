#include<stdio.h>

int main(void) {
    
    typedef struct {
        float x, y;
    } point;

    point p;
    p.x = 0.2;
    p.y = 0.43;

    printf("%f, %f\n", p.x, p.y);
    
    return 0;
}
