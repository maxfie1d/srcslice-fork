#include <stdio.h>

typedef struct Point
{
    int x;
    int y;
} Point;

Point g_point;

void f()
{
    g_point.x = 2;
    g_point.y = 3;
}

int main(void)
{
    g_point.x = 1;
    g_point.y = 1;

    f();

    return g_point.x * g_point.y;
}
