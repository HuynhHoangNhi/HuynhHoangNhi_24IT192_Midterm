
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "list.h"

/* Return non-zero if the entry should be displayed. */
static int should_include(const char *name, const Options *options)
{
    if (name[0] != '.') {
        return 1;
    }

    if (options->show_all) {
        return 1;
    }

    if (options->almost_all) {
        return strcmp(name, ".") != 0 &&
               strcmp(name, "..") != 0;
    }

    return 0;
}

/* Compare entries by name in ascending order. */
static int compare_names(const void *left, const void *right)
{
    const FileEntry *a = left;
    const FileEntry *b = right;

    return strcmp(a->name, b->name);
}

/* Compare entries by size, largest first. */
static int compare_size(const void *left, const void *right)
{
    const FileEntry *a = left;
    const FileEntry *b = right;

    if (a->metadata.st_size < b->metadata.st_size) {
        return 1;
    }

    if (a->metadata.st_size > b->metadata.st_size) {
        return -1;
    }

    return strcmp(a->name, b->name);
}

/* Return the timestamp selected by -u, -c, or the default. */
static time_t selected_time(const FileEntry *entry,
                            const Options *options)
{
    if (options->use_access_time) {
        return entry->metadata.st_atime;
    }

    if (options->use_change_time) {
        return entry->metadata.st_ctime;
    }

    return entry->metadata.st_mtime;
}

/*
 * qsort() does not accept an extra argument for its comparator.
 * Store the current options while sorting by time.
 */
static const Options *sort_options = NULL;

/* Compare selected timestamps, newest first. */
static int compare_time(const void *left, const void *right)
{
    const FileEntry *a = left;
    const FileEntry *b = right;
    time_t time_a;
    time_t time_b;

    time_a = selected_time(a, sort_options);
    time_b = selected_time(b, sort_options);

    if (time_a < time_b) {
        return 1;
    }

    if (time_a > time_b) {
        return -1;
    }

    return strcmp(a->name, b->name);
}

/* Read directory entries and collect their metadata. */
int read_directory(const char *path,
                   const Options *options,
                   FileEntry **entries,
                   size_t *count)
{
    DIR *directory;
    struct dirent *item;
    FileEntry *result = NULL;
    size_t used = 0;
    size_t capacity = 0;
    size_t path_length;

    if (path == NULL || options == NULL ||
        entries == NULL || count == NULL) {
        errno = EINVAL;
        return -1;
    }

    *entries = NULL;
    *count = 0;

    directory = opendir(path);
    if (directory == NULL) {
        return -1;
    }

    path_length = strlen(path);
    errno = 0;

    while ((item = readdir(directory)) != NULL) {
        char *name_copy;
        char *full_path;
        size_t name_length;
        size_t full_length;
        struct stat metadata;
        FileEntry *grown;

        if (!should_include(item->d_name, options)) {
            continue;
        }

        name_length = strlen(item->d_name);
        full_length = path_length + name_length + 2;

        full_path = malloc(full_length);
        if (full_path == NULL) {
            free_entries(result, used);
            closedir(directory);
            return -1;
        }

        (void)snprintf(full_path, full_length, "%s%s%s",
                       path,
                       (path_length > 0 &&
                        path[path_length - 1] == '/') ? "" : "/",
                       item->d_name);

        if (lstat(full_path, &metadata) == -1) {
            free(full_path);
            errno = 0;
            continue;
        }

        free(full_path);

        name_copy = malloc(name_length + 1);
        if (name_copy == NULL) {
            free_entries(result, used);
            closedir(directory);
            return -1;
        }

        memcpy(name_copy, item->d_name, name_length + 1);

        if (used == capacity) {
            size_t new_capacity =
                (capacity == 0) ? 16 : capacity * 2;

            grown = realloc(result,
                            new_capacity * sizeof(*result));

            if (grown == NULL) {
                free(name_copy);
                free_entries(result, used);
                closedir(directory);
                return -1;
            }

            result = grown;
            capacity = new_capacity;
        }

        result[used].name = name_copy;
        result[used].metadata = metadata;
        used++;

        errno = 0;
    }

    if (errno != 0) {
        free_entries(result, used);
        closedir(directory);
        return -1;
    }

    if (closedir(directory) == -1) {
        free_entries(result, used);
        return -1;
    }

    *entries = result;
    *count = used;

    return 0;
}

/* Sort entries according to the selected options. */
void sort_entries(FileEntry *entries,
                  size_t count,
                  const Options *options)
{
    size_t i;

    if (entries == NULL || options == NULL ||
        options->no_sort || count < 2) {
        return;
    }

    if (options->sort_size) {
        qsort(entries, count, sizeof(*entries), compare_size);
    } else if (options->sort_time) {
        sort_options = options;
        qsort(entries, count, sizeof(*entries), compare_time);
        sort_options = NULL;
    } else {
        qsort(entries, count, sizeof(*entries), compare_names);
    }

    if (options->reverse) {
        for (i = 0; i < count / 2; i++) {
            FileEntry temp = entries[i];

            entries[i] = entries[count - i - 1];
            entries[count - i - 1] = temp;
        }
    }
}

/* Free the memory allocated for directory entries. */
void free_entries(FileEntry *entries, size_t count)
{
    size_t i;

    if (entries == NULL) {
        return;
    }

    for (i = 0; i < count; i++) {
        free(entries[i].name);
    }

    free(entries);
}
