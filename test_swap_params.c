int num[10];
int numLength;
int x;

func swap(int a, int b) {
    int temp;
    temp = num[a];
    num[a] = num[b];
    num[b] = temp;
    return 0;
}

numLength = 5;
num[0] = 64;
num[1] = 34;
num[2] = 25;
num[3] = 12;
num[4] = 22;

x = 0;
while (x < numLength) {
    if (num[x] < num[x + 1]) {
        swap(x, x + 1);
    }
    x = x + 1;
}

print(num[0]);
print(num[1]);
print(num[2]);
print(num[3]);
print(num[4]);
