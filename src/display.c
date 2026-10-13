
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

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

    if (mode & S_ISUID) permissions[3] = 's';
    if (mode & S_ISGID) permissions[6] = 's';
    if (mode & S_ISVTX) permissions[9] = 't';

    printf("%s", permissions);
}

/* Append a type indicator when -F is selected. */
static char file_indicator(const struct stat *metadata)
{
    if (S_ISDIR(metadata->st_mode)) return '/';
    if (S_ISLNK(metadata->st_mode)) return '@';
    if (S_ISFIFO(metadata->st_mode)) return '|';
    if (S_ISSOCK(metadata->st_mode)) return '=';
    if (metadata->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return '*';

    return '\0';
}

/* Print the name and optionally its type indicator. */
static void print_name(const char *name,
                       const struct stat *metadata,
                       const Options *options)
{
    const unsigned char *p = (const unsigned char *)name;

    while (*p != '\0') {
        if (options->quote_nonprint && (*p < 32 || *p == 127)) {
            putchar('?');
        } else {
            putchar(*p);
        }
        p++;
    }

    if (options->classify) {
        char indicator = file_indicator(metadata);
        if (indicator != '\0') {
            putchar(indicator);
        }
    }
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

    (void)path;

    if (options->use_access_time) {
        timestamp = metadata.st_atime;
    } else if (options->use_change_time) {
        timestamp = metadata.st_ctime;
    }

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

    printf(" %8lld", (long long)metadata.st_size);

    time_info = localtime(&timestamp);
    if (time_info != NULL &&
        strftime(time_buffer, sizeof(time_buffer),
                 "%b %e %H:%M", time_info) > 0) {
        printf(" %s", time_buffer);
    } else {
        printf(" unknown-time");
    }

    printf(" ");
    print_name(entry->name, &metadata, options);
    putchar('\n');
}

/* Display one entry. */
void display_entry(const char *path,
                   const FileEntry *entry,
                   const Options *options)
{
    (void)path;

    if (entry == NULL || options == NULL) {
        return;
    }

    if (options->show_inode) {
        printf("%lu ", (unsigned long)entry->metadata.st_ino);
    }

    if (options->long_format) {
        display_long(path, entry, options);
    } else {
        print_name(entry->name, &entry->metadata, options);
        putchar('\n');
    }
}

/* Display a collection of entries. */
void display_entries(const char *path,
                     const FileEntry *entries,
                     size_t count,
                     const Options *options)
{
    size_t i;

    if (options == NULL) {
        return;
    }

    for (i = 0; i < count; i++) {
        display_entry(path, &entries[i], options);
    }
}
