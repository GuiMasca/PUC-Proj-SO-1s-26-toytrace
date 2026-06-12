#include <sys/syscall.h>

void student_format_event(const struct syscall_event *ev,
                          char *buf,
                          size_t bufsz)
{
    switch (ev->syscall_no) {

    case SYS_read:
        snprintf(buf, bufsz,
                 "read(%lu, %#lx, %lu) = %ld",
                 ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->ret);
        break;

    case SYS_write:
        snprintf(buf, bufsz,
                 "write(%lu, %#lx, %lu) = %ld",
                 ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->ret);
        break;

    case SYS_openat:
        snprintf(buf, bufsz,
                 "openat(%ld, %#lx, %#lx, %#lx) = %ld",
                 (long)ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->args[3],
                 ev->ret);
        break;

    case SYS_execve:
        snprintf(buf, bufsz,
                 "execve(%#lx, %#lx, %#lx) = %ld",
                 ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->ret);
        break;

    case SYS_exit_group:
        snprintf(buf, bufsz,
                 "exit_group(%ld) = %ld",
                 (long)ev->args[0],
                 ev->ret);
        break;

    default:
        snprintf(buf, bufsz,
                 "%s(%#lx, %#lx, %#lx, %#lx, %#lx, %#lx) = %ld",
                 syscall_name(ev->syscall_no),
                 ev->args[0],
                 ev->args[1],
                 ev->args[2],
                 ev->args[3],
                 ev->args[4],
                 ev->args[5],
                 ev->ret);
        break;
    }
}
