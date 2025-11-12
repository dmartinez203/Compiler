int globalVar;

func myFunction() {
    int localVar;
    localVar = 42;
    print(localVar);
    return localVar;
}

globalVar = 10;
print(globalVar);
myFunction();
