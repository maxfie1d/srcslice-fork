#include <stdio.h>

typedef struct Point
{
    int x;
    int y;
} Point;

Point g_point;

int f()
{
    int r = g_point.x + g_point.y;
    return r;
}

int main(void)
{
    g_point.x = 2;
    g_point.y = 3;

    int r = f();

    return r;
}
