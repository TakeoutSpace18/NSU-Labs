#define _GNU_SOURCE

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int create_dir(const char* path)
{
    if (mkdir(path, S_IRWXU) == -1)
    {
        perror("mkdir()");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int print_dir_entries(const char* path)
{
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir()");
        return EXIT_FAILURE;
    }

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL)
    {
        printf("- %s\n", entry->d_name);
    }
    closedir(dir);
    return EXIT_SUCCESS;
}

int remove_dir(const char* path)
{
    if (rmdir(path) == -1)
    {
        perror("rmdir()");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int create_file(const char* path)
{
    int fd = creat(path, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("creat()");
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

int print_file(const char* path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("open()");
        return EXIT_FAILURE;
    }

    char buf[512];
    ssize_t bytes_read;
    while((bytes_read = read(fd, buf, 512)) > 0)
    {
        write(STDOUT_FILENO, buf, bytes_read);
    }

    close(fd);
    return EXIT_SUCCESS;
}

int remove_file(const char* path)
{
    if (unlink(path) == -1)
    {
        perror("unlink()");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int create_symlink(const char* path)
{
    char* symlink_path = strdup(path);
    strcat(symlink_path, ".symlink");

    if (symlink(path, symlink_path) == -1)
    {
        perror("symlink()");
        free(symlink_path);
        return EXIT_FAILURE;
    }

    free(symlink_path);
    return EXIT_SUCCESS;
}

int read_symlink(const char* path)
{
    char buf[128];
    ssize_t read_bytes = readlink(path, buf, 128);
    if (read_bytes == -1)
    {
        perror("readlink()");
        return EXIT_FAILURE;
    }

    write()
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "expected 1 argument, provided less");
        return EXIT_FAILURE;
    }

    char* bname = basename(argv[0]);
    if (strcmp(bname, "create_dir") == 0)
    {
        return create_dir(argv[1]);
    }
    if (strcmp(bname, "print_dir_entries") == 0)
    {
        return print_dir_entries(argv[1]);
    }
    if (strcmp(bname, "remove_dir") == 0)
    {
        return remove_dir(argv[1]);
    }
    if (strcmp(bname, "create_file") == 0)
    {
        return create_file(argv[1]);
    }
    if (strcmp(bname, "print_file") == 0)
    {
        return print_file(argv[1]);
    }
    if (strcmp(bname, "remove_file") == 0)
    {
        return remove_file(argv[1]);
    }
    if (strcmp(bname, "create_symlink") == 0)
    {
        return create_symlink(argv[1]);
    }
    if (strcmp(bname, "print_symlink") == 0)
    {
        return read_symlink(argv[1]);
    }

    fprintf(stderr, "unrecognized executable name");
    return EXIT_FAILURE;
}
