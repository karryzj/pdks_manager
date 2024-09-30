#ifndef LAYER_COMMON_H
#define LAYER_COMMON_H

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_LAYER_LIBRARY
#     define LY_PUBLIC __declspec(dllexport)
#   else
#     define LY_PUBLIC __declspec(dllimport)
#   endif
#   define LY
#   define LY_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define LY_PUBLIC __attribute__ ((visibility ("default")))
#     define LY_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define LY_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define LY_PUBLIC
#     define LY_PUBLIC_TEMPLATE
#     define LY_LOCAL
#   endif

# endif

#endif // LAYER_COMMON_H

