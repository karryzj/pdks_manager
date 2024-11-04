#ifndef CMDCOMMON_H
#define CMDCOMMON_H

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_CMD_LIBRARY
#     define CMD_PUBLIC __declspec(dllexport)
#   else
#     define CMD_PUBLIC __declspec(dllimport)
#   endif
#   define CMD_LOCAL
#   define CMD_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define CMD_PUBLIC __attribute__ ((visibility ("default")))
#     define CMD_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define CMD_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define CMD_PUBLIC
#     define CMD_PUBLIC_TEMPLATE
#     define CMD_LOCAL
#   endif

# endif

#endif // CMDCOMMON_H
