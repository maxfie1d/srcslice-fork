int main(void)
{
    int sum = 0;
    int n = 0;
    int data = read();
    while (data > 0)
    {
        sum = sum + data;
        n = n + 1;
        data = read();
    }
    printf("sum: %d\n", sum);
    printf("n: %d\n", n);
    int average = sum / n;
    printf("average: %d\n", average);

    return 0;
}
