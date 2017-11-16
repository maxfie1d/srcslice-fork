int f(int a[]) {
    const int b[] = {1, 2, 3};

    return a[0] + b[0];
}

int main(void) {
    int x[] = {4, 5, 6};
    return f(x);
}
