int add(int a, int b);

int result_holder;

int add(int a, int b) {
    int sum;
    sum = a + b;
    return sum;
}

int main() {
    int x;
    int y;
    x = 10;
    y = 5;

    result_holder = add(x, y);

    print(add(result_holder, 1));
    
    return 0;
}