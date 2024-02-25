#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int trace_syscalls(pid_t tracee_pid)
{
    int status;

    // wait for tracee to call PTRACE_TRACEME
    if (waitpid(tracee_pid, &status, 0) < 0)
    {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (!WIFSTOPPED(status) || WSTOPSIG(status) != SIGSTOP) {
        fprintf(stderr, "tracer: unexpected wait status: %x", status);
    }

    if (ptrace(PTRACE_SYSCALL, tracee_pid, &status, NULL) < 0)
    {
        perror("ptrace(PTRACE_SYSCALL)");
        return EXIT_FAILURE;
    }

    while (waitpid(tracee_pid, &status, 0) > 0 && !WIFEXITED(status))
    {
        struct __ptrace_syscall_info syscall_info;

        if (ptrace(PTRACE_GET_SYSCALL_INFO, tracee_pid, sizeof(syscall_info), &syscall_info) < 0)
        {
            perror("ptrace(PTRACE_GET_SYSCALL_INFO)");
            return EXIT_FAILURE;
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

int launch_child(const char* file, char** argv)
{
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0)
    {
        perror("ptrace");
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
            launch_child(tracee_path, &argv[1]);
            break;
        default:
            trace_syscalls(pid);
    }
    return EXIT_SUCCESS;
}
