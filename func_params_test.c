func add(int a, int b) { int temp; temp = a + b; return temp; }
func inc(int n) { int temp; temp = n + 1; return temp; }

int x; int y; int r1; int r2;
x = 2; y = 3;
r1 = add(x, y);
r2 = inc(r1);
print(r1);
print(r2);
