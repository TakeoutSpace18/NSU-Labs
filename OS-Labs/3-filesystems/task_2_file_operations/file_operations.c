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

int print_symlink(const char* path)
{
    char buf[128];
    ssize_t read_bytes = readlink(path, buf, 128);
    if (read_bytes == -1)
    {
        perror("readlink()");
        return EXIT_FAILURE;
    }

    write(STDOUT_FILENO, buf, read_bytes);
    return EXIT_SUCCESS;
}

int create_hardlink(const char* path)
{
    char* hardlink_path = strdup(path);
    strcat(hardlink_path, ".hardlink");

    if (link(path, hardlink_path) == -1)
    {
        perror("link()");
        free(hardlink_path);
        return EXIT_FAILURE;
    }

    free(hardlink_path);
    return EXIT_SUCCESS;
}

int print_permissions(const char* path)
{
    struct stat file_stat;
    if (stat(path, &file_stat) == -1)
    {
        perror("stat()");
        return EXIT_FAILURE;
    }

    printf("permissions: %o\n", file_stat.st_mode & 0777);
    printf("link count: %lu\n", file_stat.st_nlink);
    return EXIT_SUCCESS;
}

int parse_permissions_string(const char* str)
{
    int permissions = 0;

    char* pattern = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i)
    {
        permissions <<= 1;
        if (str[i] == pattern[i])
        {
            permissions |= 0x1;
        }
        else if (str[i] != '-')
        {
            return -1;
        }
    }

    return permissions;
}

int change_permissions(const char* path, const char* permissions_str)
{
    int permissions = parse_permissions_string(permissions_str);
    if (permissions == -1)
    {
        fprintf(stderr, "Failed to parse permissions string");
        return EXIT_FAILURE;
    }

    struct stat file_stat;
    if (stat(path, &file_stat) == -1)
    {
        perror("stat()");
        return EXIT_FAILURE;
    }

    mode_t new_mode = (file_stat.st_mode & ~0777) | permissions;
    if (chmod(path, new_mode) == -1)
    {
        perror("chmod()");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
        return print_symlink(argv[1]);
    }
    if (strcmp(bname, "create_hardlink") == 0)
    {
        return create_hardlink(argv[1]);
    }
    if (strcmp(bname, "print_permissions") == 0)
    {
        return print_permissions(argv[1]);
    }
    if (strcmp(bname, "change_permissions") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, "expected 2 arguments, provided less");
            return EXIT_FAILURE;
        }
        return change_permissions(argv[1], argv[2]);
    }

    fprintf(stderr, "unrecognized executable name");
    return EXIT_FAILURE;
}
