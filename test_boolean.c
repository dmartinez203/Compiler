int x;
int y;
int z;
int result;

x = 5;
y = 10;
z = 15;

if (x < y && y < z) {
    result = 1;
    print(result);
}

if (x > z || y == 10) {
    result = 2;
    print(result);
}

if (!(x > y)) {
    result = 3;
    print(result);
}

if ((x < y) && (y < z) && (x < z)) {
    result = 4;
    print(result);
}
