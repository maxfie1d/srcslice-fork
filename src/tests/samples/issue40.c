typedef struct object
{
    int x;
} object;

int main()
{
    object o;
    o.x = 3;
    if (o.x > 0 &&
        o.x < 10)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
