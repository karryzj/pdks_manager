#ifndef PRICOMMON_H
#define PRICOMMON_H

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_PRI_LIBRARY
#     define PRI_PUBLIC __declspec(dllexport)
#   else
#     define PRI_PUBLIC __declspec(dllimport)
#   endif
#   define PRI
#   define PRI_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define PRI_PUBLIC __attribute__ ((visibility ("default")))
#     define PRI_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define PRI_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define PRI_PUBLIC
#     define PRI_PUBLIC_TEMPLATE
#     define PRI_LOCAL
#   endif

# endif

#endif // PRICOMMON_H
