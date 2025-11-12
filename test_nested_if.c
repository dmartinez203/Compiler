int x;
int y;
int z;
int result;

x = 10;
y = 5;
z = 15;

if (x > y) {
    if (z > x) {
        result = 1;
    } else {
        result = 2;
    }
} else {
    result = 0;
}

print(result);
 