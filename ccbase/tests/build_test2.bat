gcc utils\compile_bake.c tests\test2_impl.c -o bake.exe
bake.exe
gcc baked.c tests\test2.c -o test2.exe
test2.exe
del bake.exe baked.c test2.exe