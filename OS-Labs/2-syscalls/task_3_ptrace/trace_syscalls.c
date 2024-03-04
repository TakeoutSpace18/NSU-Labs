#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <sys/wait.h>
#include <seccomp.h>

void print_syscall_info(const struct ptrace_syscall_info* info)
{
    if (info->op == PTRACE_SYSCALL_INFO_ENTRY)
    {
        char* syscall_name = seccomp_syscall_resolve_num_arch(SCMP_ARCH_NATIVE, info->entry.nr);
        printf("%03llu:   %s(", info->entry.nr, syscall_name);
        free(syscall_name);

        for (int i = 0; i < 5; i++)
        {
            printf("0x%llx, ", info->entry.args[i]);
        }
        printf("0x%llx)", info->entry.args[5]);
    }

    else if (info->op == PTRACE_SYSCALL_INFO_EXIT)
    {
        printf(" --> 0x%llx ", info->exit.rval);
        if (!info->exit.is_error)
        {
            puts("(SUCCESS)");
        }
        else
        {
            puts("(ERROR)");
        }
    }
}

int tracer(pid_t tracee_pid)
{
    int status;

    // wait for tracee to call PTRACE_TRACEME
    if (waitpid(tracee_pid, &status, 0) < 0)
    {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (!WIFSTOPPED(status) || WSTOPSIG(status) != SIGSTOP)
    {
        fprintf(stderr, "tracer: unexpected wait status: %x", status);
    }

    ptrace(PTRACE_SETOPTIONS, tracee_pid, 0, PTRACE_O_TRACESYSGOOD);

    if (ptrace(PTRACE_SYSCALL, tracee_pid, &status, NULL) < 0)
    {
        perror("ptrace(PTRACE_SYSCALL)");
        return EXIT_FAILURE;
    }

    while (waitpid(tracee_pid, &status, 0) > 0 && !WIFEXITED(status))
    {
        if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
        {
            struct ptrace_syscall_info syscall_info;

            if (ptrace(PTRACE_GET_SYSCALL_INFO, tracee_pid, sizeof(syscall_info), &syscall_info) < 0)
            {
                perror("ptrace(PTRACE_GET_SYSCALL_INFO)");
                return EXIT_FAILURE;
            }

            print_syscall_info(&syscall_info);
        }

        // continue tracee and stop on syscall
        if (ptrace(PTRACE_SYSCALL, tracee_pid, &status, NULL) < 0)
        {
            perror("ptrace(PTRACE_SYSCALL)");
            return EXIT_FAILURE;
        }
    }

    return WEXITSTATUS(status);
}

int tracee(const char* file, char** argv)
{
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0)
    {
        perror("ptrace(TRACEME)");
        return EXIT_FAILURE;
    }

    if (raise(SIGSTOP) != 0)
    {
        perror("raise(SIGSEGV)");
        return EXIT_FAILURE;
    }

    if (execvp(file, argv) < 0)
    {
        perror("execvp");
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid arguments count: tracee was not specified\n");
        return EXIT_FAILURE;
    }

    const char* tracee_path = argv[1];

    pid_t pid = fork();
    switch (pid)
    {
        case -1:
            perror("fork");
            return EXIT_FAILURE;
        case 0:
            tracee(tracee_path, &argv[1]);
            break;
        default:
            tracer(pid);
    }
    return EXIT_SUCCESS;
}
