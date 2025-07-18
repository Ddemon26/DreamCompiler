#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#include <direct.h>
#define DR_PATH_SEP '\\'
#define DR_PATH_SEP_STR "\\"
#define DR_NEWLINE "\r\n"
#define dr_mkdir(p) _mkdir(p)
#define dr_unlink(p) remove(p)
#else
#include <sys/stat.h>
#include <unistd.h>
#define DR_PATH_SEP '/'
#define DR_PATH_SEP_STR "/"
#define DR_NEWLINE "\n"
#define dr_mkdir(p) mkdir(p, 0755)
#define dr_unlink(p) unlink(p)
#endif

#endif // PLATFORM_H
