
#if !defined(HDR_spCommon_h)
# define HDR_spCommon_h

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_SP_LIBRARY
#     define SP_PUBLIC __declspec(dllexport)
#   else
#     define SP_PUBLIC __declspec(dllimport)
#   endif
#   define SP_LOCAL
#   define SP_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define SP_PUBLIC __attribute__ ((visibility ("default")))
#     define SP_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define SP_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define SP_PUBLIC
#     define SP_PUBLIC_TEMPLATE
#     define SP_LOCAL
#   endif

# endif

#endif
