
#if !defined(HDR_pmCommon_h)
# define HDR_pmCommon_h

# if defined _WIN32 || defined __CYGWIN__

#   ifdef MAKE_PM_LIBRARY
#     define PM_PUBLIC __declspec(dllexport)
#   else
#     define PM_PUBLIC __declspec(dllimport)
#   endif
#   define PM_LOCAL
#   define PM_PUBLIC_TEMPLATE

# else

#   if __GNUC__ >= 4 || defined(__clang__)
#     define PM_PUBLIC __attribute__ ((visibility ("default")))
#     define PM_PUBLIC_TEMPLATE __attribute__ ((visibility ("default")))
#     define PM_LOCAL  __attribute__ ((visibility ("hidden")))
#   else
#     define PM_PUBLIC
#     define PM_PUBLIC_TEMPLATE
#     define PM_LOCAL
#   endif

# endif

#endif
