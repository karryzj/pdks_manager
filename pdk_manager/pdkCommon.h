#ifndef PDKCOMMON_H
#define PDKCOMMON_H

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_PDK_LIBRARY
#     define PDK_PUBLIC __declspec(dllexport)
#   else
#     define PDK_PUBLIC __declspec(dllimport)
#   endif
#   define PDK
#   define PDK_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define PDK_PUBLIC __attribute__ ((visibility ("default")))
#     define PDK_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define PDK_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define PDK_PUBLIC
#     define PDK_PUBLIC_TEMPLATE
#     define PDK_LOCAL
#   endif

# endif

#endif // PDKCOMMON_H
