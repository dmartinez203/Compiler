func func_one() { int local_var; local_var = 100; return local_var; }

func func_two() { int local_var; local_var = 200; return local_var; }

int result_one; int result_two;

result_one = func_one(); result_two = func_two();

print(result_one); print(result_two);

print(result_one + result_two);