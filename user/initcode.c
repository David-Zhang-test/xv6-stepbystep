#include "../kernel/types.h"
#include "stdarg.h"

extern int fork(void);
extern int exit(int) __attribute__((noreturn));
extern int wait(int *);
extern int write(int, const void *, int);
extern int kill(int);
extern int getpid(void);
extern char *sbrk(int);
extern int sleep(int);
extern int uptime(void);

// -----------------------------------------------------------------------
//                               sbrk相关
// -----------------------------------------------------------------------

typedef long Align;

union header
{
    struct
    {
        union header *ptr;
        uint size;
    } s;
    Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

uint64 heaptop = 0;

static Header *add_heaptop(int nu);

void free(void *ap)
{
    Header *bp, *p;

    bp = (Header *)ap - 1;
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;
    if (bp + bp->s.size == p->s.ptr)
    {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp)
    {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
        bp = p;
    }
    else
        p->s.ptr = bp;
    freep = p;
    // bp is the last block and larger than 16KB, shrink to 4KB
    if ((uint64)(bp + bp->s.size) >= heaptop && bp->s.size >= 1024)
    {
        add_heaptop(256 - bp->s.size);
        bp->s.size = 256;
    }
}

static Header *add_heaptop(int nu)
{
    char *p;
    Header *hp;

    if (nu > 0)
    {
        p = sbrk(nu * sizeof(Header));
        if (p == (char *)-1)
            return 0;
        hp = (Header *)p;
        hp->s.size = nu;
        heaptop = (uint64)hp + nu * sizeof(Header);
        free((void *)(hp + 1));
        return freep;
    }
    else
    {
        p = sbrk(nu * sizeof(Header));
        if (p == (char *)-1)
            return 0;
        heaptop += nu * sizeof(Header);
        return 0;
    }
}

void *malloc(uint nbytes)
{
    Header *p, *prevp;
    uint nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == 0)
    {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr)
    {
        if (p->s.size >= nunits)
        {
            if (p->s.size == nunits)
                prevp->s.ptr = p->s.ptr;
            else
            {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }
        if (p == freep)
            // grow, by 4KB at the minimum
            if ((p = add_heaptop(nunits < 256 ? 256 : nunits)) == 0)
                return 0;
    }
}

// -----------------------------------------------------------------------
//                              printf相关
// -----------------------------------------------------------------------

static char digits[] = "0123456789ABCDEF";

static void putc(int fd, char c)
{
    write(fd, &c, 1);
}

static void printint(int fd, int xx, int base, int sgn)
{
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if (sgn && xx < 0)
    {
        neg = 1;
        x = -xx;
    }
    else
    {
        x = xx;
    }

    i = 0;
    do
    {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0)
        putc(fd, buf[i]);
}

static void printptr(int fd, uint64 x)
{
    int i;
    putc(fd, '0');
    putc(fd, 'x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void vprintf(int fd, const char *fmt, va_list ap)
{
    char *s;
    int c, i, state;

    state = 0;
    for (i = 0; fmt[i]; i++)
    {
        c = fmt[i] & 0xff;
        if (state == 0)
        {
            if (c == '%')
            {
                state = '%';
            }
            else
            {
                putc(fd, c);
            }
        }
        else if (state == '%')
        {
            if (c == 'd')
            {
                printint(fd, va_arg(ap, int), 10, 1);
            }
            else if (c == 'l')
            {
                printint(fd, va_arg(ap, uint64), 10, 0);
            }
            else if (c == 'x')
            {
                printint(fd, va_arg(ap, int), 16, 0);
            }
            else if (c == 'p')
            {
                printptr(fd, va_arg(ap, uint64));
            }
            else if (c == 's')
            {
                s = va_arg(ap, char *);
                if (s == 0)
                    s = "(null)";
                while (*s != 0)
                {
                    putc(fd, *s);
                    s++;
                }
            }
            else if (c == 'c')
            {
                putc(fd, va_arg(ap, uint));
            }
            else if (c == '%')
            {
                putc(fd, c);
            }
            else
            {
                // Unknown % sequence.  Print it to draw attention.
                putc(fd, '%');
                putc(fd, c);
            }
            state = 0;
        }
    }
}

void printf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(1, fmt, ap);
}

// -----------------------------------------------------------------------
//                                测试
// -----------------------------------------------------------------------

int main()
{
    int pid, to_be_killed;

    // child 1: 100% CPU
    // test for: fork, time slice, kill
    pid = fork();
    if (pid < 0)
    {
        printf("[pid: %d] 1st fork() failed", getpid());
        return 0;
    }
    if (pid == 0)
    {
        write(3,0,0);
        while (1)
            ;
        exit(0);
    }
    to_be_killed = pid;

    // child 2: dynamic memory allocation
    // test for: fork, getpid, uptime, write, sbrk, exit
    pid = fork();
    if (pid < 0)
    {
        printf("[pid: %d] 2nd fork() failed", getpid());
        return 0;
    }
    if (pid == 0)
    {
        int *p[10];
        for (int i = 0; i < 10; i++)
            p[i] = malloc((i + 1) * 1000);
        // --------
        free(p[0]);
        free(p[2]);
        free(p[4]);
        free(p[6]);
        free(p[8]);
        // --------
        free(p[9]);
        free(p[7]);
        free(p[5]);
        free(p[3]);
        free(p[1]);
        printf("[pid: %d] sbrk test over! uptime: %d\n", getpid(), uptime());
        exit(0);
    }

    // child 3: sleep test
    // test for: fork, getpid, uptime, write, sleep, exit, kill
    pid = fork();
    if (pid < 0)
    {
        printf("[pid: %d] 2nd fork() failed", getpid());
        return 0;
    }
    if (pid == 0)
    {
        sleep(10);
        printf("[pid: %d] sleep test - 1s. uptime: %d\n", getpid(), uptime());
        sleep(20);
        printf("[pid: %d] sleep test - 2s. uptime: %d\n", getpid(), uptime());
        sleep(30);
        printf("[pid: %d] sleep test - 3s. uptime: %d\n", getpid(), uptime());
        kill(to_be_killed);
        exit(0);
    }
    // init process: wait
    // test for: wait, getpid, uptime, write
    while (1)
    {
        int pid, xstate;
        pid = wait(&xstate);
        if (pid < 0)
            break;
        printf("[pid: %d] wait - pid: %d - exit code: %d\n", getpid(), pid, xstate);
    }
    printf("[pid: %d] uptime: %d, init process has no child now, over!\n", getpid(), uptime());
    while (1)
        ;
    return 0;
}
