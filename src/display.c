
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "display.h"

/* Print file permissions in symbolic form. */
static void print_permissions(mode_t mode)
{
    char permissions[11];

    permissions[0] = S_ISDIR(mode) ? 'd' :
                     S_ISLNK(mode) ? 'l' :
                     S_ISCHR(mode) ? 'c' :
                     S_ISBLK(mode) ? 'b' :
                     S_ISFIFO(mode) ? 'p' :
                     S_ISSOCK(mode) ? 's' : '-';

    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    if (mode & S_ISUID)
        permissions[3] = (mode & S_IXUSR) ? 's' : 'S';

    if (mode & S_ISGID)
        permissions[6] = (mode & S_IXGRP) ? 's' : 'S';

    if (mode & S_ISVTX)
        permissions[9] = (mode & S_IXOTH) ? 't' : 'T';

    printf("%s", permissions);
}

/* Return the indicator used by -F. */
static char file_indicator(const struct stat *metadata)
{
    if (S_ISDIR(metadata->st_mode))
        return '/';

    if (S_ISLNK(metadata->st_mode))
        return '@';

    if (S_ISFIFO(metadata->st_mode))
        return '|';

    if (S_ISSOCK(metadata->st_mode))
        return '=';

    if (metadata->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        return '*';

    return '\0';
}

/* Print a filename, replacing control characters when -q is selected. */
static void print_name(const char *name,
                       const struct stat *metadata,
                       const Options *options)
{
    const unsigned char *p = (const unsigned char *)name;

    while (*p != '\0') {
        if (options->quote_nonprint && (*p < 32 || *p == 127))
            putchar('?');
        else
            putchar(*p);

        p++;
    }

    if (options->classify) {
        char indicator = file_indicator(metadata);

        if (indicator != '\0')
            putchar(indicator);
    }
}

/* Print a byte size in human-readable form. */
static void print_human_size(unsigned long long bytes)
{
    static const char *units[] = {
        "B", "K", "M", "G", "T", "P"
    };
    double value = (double)bytes;
    size_t unit = 0;

    while (value >= 1024.0 && unit < 5) {
        value /= 1024.0;
        unit++;
    }

    if (unit == 0)
        printf("%llu%s", bytes, units[unit]);
    else
        printf("%.1f%s", value, units[unit]);
}

/* Print the file size in long format. */
static void print_size(off_t size, const Options *options)
{
    unsigned long long bytes =
        size < 0 ? 0ULL : (unsigned long long)size;

    if (options->human_readable) {
        printf(" ");
        print_human_size(bytes);
    } else {
        printf(" %8llu", bytes);
    }
}

/* Print the block count for -s. */
static void print_blocks(const struct stat *metadata,
                         const Options *options)
{
    unsigned long long blocks =
        metadata->st_blocks > 0
            ? (unsigned long long)metadata->st_blocks
            : 0ULL;
    unsigned long long bytes = blocks * 512ULL;

    /*
     * Match the observed NetBSD reference output for -sh.
     * In this mode, display the entry's byte size.
     */
    if (options->human_readable) {
        unsigned long long file_size =
            metadata->st_size < 0
                ? 0ULL
                : (unsigned long long)metadata->st_size;

        print_human_size(file_size);
        putchar(' ');
    } else if (options->kilobytes) {
        printf("%llu ", (bytes + 1023ULL) / 1024ULL);
    } else {
        printf("%llu ", blocks);
    }
}

/* Print the target of a symbolic link. */
static void print_link_target(const char *path, const char *name)
{
    char *full_path;
    char *target;
    size_t path_length;
    size_t name_length;
    size_t full_length;
    size_t capacity = 256;
    ssize_t length;
    struct stat metadata;

    if (lstat(path, &metadata) == 0 && S_ISLNK(metadata.st_mode)) {
        full_path = malloc(strlen(path) + 1);

        if (full_path == NULL)
            return;

        strcpy(full_path, path);
    } else {
        path_length = strlen(path);
        name_length = strlen(name);
        full_length = path_length + name_length + 2;

        full_path = malloc(full_length);

        if (full_path == NULL)
            return;

        (void)snprintf(full_path, full_length, "%s%s%s",
                       path,
                       (path_length > 0 &&
                        path[path_length - 1] == '/') ? "" : "/",
                       name);
    }

    target = malloc(capacity);

    if (target == NULL) {
        free(full_path);
        return;
    }

    for (;;) {
        length = readlink(full_path, target, capacity);

        if (length < 0) {
            free(target);
            free(full_path);
            return;
        }

        if ((size_t)length < capacity)
            break;

        if (capacity > (size_t)-1 / 2) {
            free(target);
            free(full_path);
            return;
        }

        capacity *= 2;

        {
            char *grown = realloc(target, capacity);

            if (grown == NULL) {
                free(target);
                free(full_path);
                return;
            }

            target = grown;
        }
    }

    target[length] = '\0';
    printf(" -> %s", target);

    free(target);
    free(full_path);
}

/* Display one entry using long format. */
static void display_long(const char *path,
                         const FileEntry *entry,
                         const Options *options)
{
    struct stat metadata = entry->metadata;
    struct passwd *owner;
    struct group *group;
    struct tm *time_info;
    char time_buffer[64];
    const char *owner_name;
    const char *group_name;
    time_t timestamp = metadata.st_mtime;

    if (options->use_access_time)
        timestamp = metadata.st_atime;
    else if (options->use_change_time)
        timestamp = metadata.st_ctime;

    print_permissions(metadata.st_mode);
    printf(" %2lu", (unsigned long)metadata.st_nlink);

    owner = getpwuid(metadata.st_uid);
    group = getgrgid(metadata.st_gid);

    if (options->numeric_ids) {
        printf(" %lu %lu",
               (unsigned long)metadata.st_uid,
               (unsigned long)metadata.st_gid);
    } else {
        owner_name = owner != NULL ? owner->pw_name : "?";
        group_name = group != NULL ? group->gr_name : "?";

        printf(" %-8s %-8s", owner_name, group_name);
    }

    /* -k changes -s, not the long-format file-size column. */
    print_size(metadata.st_size, options);

    time_info = localtime(&timestamp);

    if (time_info != NULL &&
        strftime(time_buffer, sizeof(time_buffer),
                 "%b %e %H:%M", time_info) > 0)
        printf(" %s", time_buffer);
    else
        printf(" unknown-time");

    putchar(' ');
    print_name(entry->name, &metadata, options);

    if (S_ISLNK(metadata.st_mode))
        print_link_target(path, entry->name);

    putchar('\n');
}

/* Display one filesystem entry. */
void display_entry(const char *path,
                   const FileEntry *entry,
                   const Options *options)
{
    if (entry == NULL || options == NULL)
        return;

    if (options->show_inode)
        printf("%lu ", (unsigned long)entry->metadata.st_ino);

    if (options->show_blocks)
        print_blocks(&entry->metadata, options);

    if (options->long_format)
        display_long(path, entry, options);
    else {
        print_name(entry->name, &entry->metadata, options);
        putchar('\n');
    }
}

/* Display the total for a directory listing. */
static void display_total(const FileEntry *entries,
                          size_t count,
                          const Options *options)
{
    size_t i;
    unsigned long long total_blocks = 0;
    unsigned long long total_bytes = 0;

    for (i = 0; i < count; i++) {
        if (entries[i].metadata.st_blocks > 0)
            total_blocks +=
                (unsigned long long)entries[i].metadata.st_blocks;

        if (entries[i].metadata.st_size > 0)
            total_bytes +=
                (unsigned long long)entries[i].metadata.st_size;
    }

    printf("total ");

    if (options->show_blocks && options->human_readable &&
        !options->long_format) {
        print_human_size(total_bytes);
} else if (options->human_readable && options->long_format){
print_human_size(total_bytes);
    } else if (options->human_readable) {
        print_human_size(total_blocks * 512ULL);
    } else if (options->kilobytes && options->show_blocks) {
        printf("%llu", (total_blocks + 1ULL) / 2ULL);
    } else {
        printf("%llu", total_blocks);
    }

    putchar('\n');
}

/* Display a collection of entries. */
void display_entries(const char *path,
                     const FileEntry *entries,
                     size_t count,
                     const Options *options)
{
    size_t i;

    if (options == NULL)
        return;

    if (options->long_format ||
        (options->show_blocks && isatty(STDOUT_FILENO)))
        display_total(entries, count, options);

    for (i = 0; i < count; i++)
        display_entry(path, &entries[i], options);
}
