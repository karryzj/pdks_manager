#ifndef ATCOMMON_H
#define ATCOMMON_H

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_AT_LIBRARY
#     define AT_PUBLIC __declspec(dllexport)
#   else
#     define AT_PUBLIC __declspec(dllimport)
#   endif
#   define AT_LOCAL
#   define AT_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define AT_PUBLIC __attribute__ ((visibility ("default")))
#     define AT_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define AT_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define AT_PUBLIC
#     define AT_PUBLIC_TEMPLATE
#     define AT_LOCAL
#   endif

# endif

#endif // ATCOMMON_H
