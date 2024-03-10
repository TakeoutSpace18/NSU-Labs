#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

void copy_file_backwards_at(int src_dirfd, const char* src_path, int dst_dirfd, const char* dst_path);

int copy_dir_backwards_at(int src_dirfd, const char* src_path, int dst_dirfd, const char* dst_path);

char* get_reversed_path_basename(char* path);

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("usage: copy_backwards <src_path> <dst_path>");
        return EXIT_FAILURE;
    }

    int dst_dirfd = open(argv[2], O_DIRECTORY | O_RDONLY);
    char* reversed_basename = get_reversed_path_basename(argv[1]);
    copy_dir_backwards_at(AT_FDCWD, argv[1], dst_dirfd, reversed_basename);
    free(reversed_basename);
    return EXIT_SUCCESS;
}

int copy_dir_backwards_at(int src_dirfd, const char* src_path, int dst_dirfd, const char* dst_path)
{
    int src_fd = openat(src_dirfd, src_path, O_DIRECTORY | O_RDONLY);
    if (src_fd == -1)
    {
        perror("openat(src_path)");
        goto finish;
    }

    DIR* src_dir = fdopendir(src_fd);
    if (!src_dir)
    {
        perror("opendir()");
        goto finish;
    }

    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1)
    {
        perror("fstat(src_fd)");
        goto finish;
    }

    if (mkdirat(dst_dirfd, dst_path, src_stat.st_mode) == -1)
    {
        perror("mkdirat(dst_path)");
        goto finish;
    }

    int dst_fd = openat(dst_dirfd, dst_path, O_RDONLY | O_DIRECTORY);
    if (dst_fd == -1)
    {
        perror("openat(dst_path)!!");
        goto finish;
    }

    struct dirent* entry;
    while ((entry = readdir(src_dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            char* reversed_name = get_reversed_path_basename(entry->d_name);
            copy_file_backwards_at(src_fd, entry->d_name, dst_fd, reversed_name);
            free(reversed_name);
        }
        else if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            char* reversed_name = get_reversed_path_basename(entry->d_name);
            copy_dir_backwards_at(src_fd, entry->d_name, dst_fd, reversed_name);
            free(reversed_name);
        }
        else
        {
            printf("%s/%s is not a regular file: skipping.\n", src_path, entry->d_name);
        }
    }

finish:
    closedir(src_dir);
    close(src_fd);
}

void copy_file_backwards_at(int src_dirfd, const char* src_path, int dst_dirfd, const char* dst_path)
{
    int src_fd = openat(src_dirfd, src_path, O_RDONLY);
    if (src_fd == -1)
    {
        perror("openat(src_path)");
        goto finish;
    }

    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1)
    {
        perror("fstat(src_fd)");
        goto finish;
    }

    int dst_fd = openat(dst_dirfd, dst_path, O_CREAT | O_TRUNC | O_WRONLY, src_stat.st_mode);
    if (dst_fd == -1)
    {
        perror("openat(dst_path)");
        goto finish;
    }

    off_t src_size = lseek(src_fd, 0, SEEK_END);
    if (src_size == -1)
    {
        perror("lseek(SEEK_END)");
        goto finish;
    }

    for (off_t i = 0; i < src_size; i++)
    {
        off_t src_offset = src_size - 1 - i;
        off_t dst_offset = i;
        ssize_t status = copy_file_range(src_fd, &src_offset, dst_fd, &dst_offset, 1, 0);
        if (status != 1)
        {
            perror("copy_file_range()");
            goto finish;
        }
    }

finish:
    close(src_fd);
    close(dst_fd);
}

void reverse_substring(char* begin, char* end)
{
    while (begin < end)
    {
        char tmp = *begin;
        *begin = *end;
        *end = tmp;
        begin++;
        end--;
    }
}

char* get_path_basename(char* path)
{
    char* delim = strrchr(path, '/');
    char* basename = delim ? delim + 1 : path;
    return strdup(basename);
}

char* get_reversed_path_basename(char* path)
{
    char* bname = get_path_basename(path);
    reverse_substring(bname, bname + strlen(bname) - 1);
    return bname;
}
