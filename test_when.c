int x;
int y;
int result;

x = 10;
y = 5;

when (x > y) {
    result = 1;
    print(result);
} else {
    result = 0;
    print(result);
}

when (x == 10 && y == 5) {
    result = 100;
    print(result);
}

int a;
int b;
a = 3;
b = 7;

when (a < b) {
    when (b > 5) {
        result = 200;
        print(result);
    }
}
