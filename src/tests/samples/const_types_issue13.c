int f(const int a) {
    const int b = 0;

    return a + b;
}

int main(void) {
    return f(0);
}
