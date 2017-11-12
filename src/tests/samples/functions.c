// 関数一覧を取得できることをテストするためのソース

#include <stdio.h>

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

void do_nothing()
{
}

void echo(const char *s)
{
    printf("%s\n", s);
}

int main(void)
{
    echo("Hello");
    return 0;
}
