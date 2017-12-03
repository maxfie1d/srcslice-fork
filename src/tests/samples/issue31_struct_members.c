#include <stdio.h>

typedef struct Point 
{
    int x;
    int y;
} Point;

int main(void)
{
    Point point;

    point.x = 2;
    point.y = 3;

    return point.x * point.y;
}
