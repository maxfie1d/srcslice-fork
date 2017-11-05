int g = 0;
int f(int x)
{
    int c = x + g;
    g = c;
    return c;
}
int main()
{
    int a = 1;
    int b = 2;
    a = f(a);
    b = g;
    return 0;
}
