long foo(long x){
    return (x >> 40) + x;
}

int main() {
    long num = 0;
    while(num != 0xffffffffffffffff){
        if (0x8000000000000000 == foo(num)){
            printf("%ld", num);
        }
        num++;
    }
    return 0;
}