int x;
int y;
int result;

func max() {
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

x = 15;
y = 20;
result = max();
print(result);

x = 30;
y = 25;
result = max();
print(result);
