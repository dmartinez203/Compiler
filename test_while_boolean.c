int a;
int b;
int c;
int d;

a = 10;
b = 5;
c = 20;
d = 10;

while ((a > b) && (c > d)) {
   write('r');
   write('=');
   write((a > b) && (c > d));
   writeln;
   a = a - 1;
}