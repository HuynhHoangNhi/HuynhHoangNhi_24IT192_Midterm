
#ifndef DISPLAY_H
#define DISPLAY_H

#include "list.h"
#include "options.h"

/* Display one filesystem entry. */
void display_entry(const char *path,
                   const FileEntry *entry,
                   const Options *options);

/* Display a collection of entries. */
void display_entries(const char *path,
                     const FileEntry *entries,
                     size_t count,
                     const Options *options);

#endif
