#ifndef COMMON_H
#define COMMON_H

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_CM_LIBRARY
#     define CM_PUBLIC __declspec(dllexport)
#   else
#     define CM_PUBLIC __declspec(dllimport)
#   endif
#   define CM
#   define CM_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define CM_PUBLIC __attribute__ ((visibility ("default")))
#     define CM_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define CM_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define CM_PUBLIC
#     define CM_PUBLIC_TEMPLATE
#     define CM_LOCAL
#   endif

# endif

#endif // ATCOMMON_H

