extern int exit(int) __attribute__((noreturn));
extern int write(int, const void *, int);
int main() {
    write(3, 0, 0); // 3 is testcode for block r/w, see sys_write()
    while (1)
        ;
    exit(0);
}