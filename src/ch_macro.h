#ifndef _CH_MACRO_H
#define _CH_MACRO_H

#if defined(_WIN32)
#ifdef CH_COMPILE_LIBRARY
#define CH_CAPI_EXPORT __declspec(dllexport)
#else
#define CH_CAPI_EXPORT __declspec(dllimport)
#endif // CH_COMPILE_LIBRARY
#else
#define CH_CAPI_EXPORT __attribute__((visibility("default")))
#endif // _WIN32

#define CH_PASTE(a, b) a##b
#define CH_CAT(a, b) CH_PASTE(a, b)
#define CH_JOIN(a, b) CH_CAT(a, CH_CAT(_, b))

#define CH_API_PREFIX ch

#define CH_V(n, v) __typeof__(v) n = v
#define CH_VAR_LIST(X, l) ch_list_##X l = ch_list_##X##_init()

#define CH_LIST_FOREACH(X, l, cur)                                                   \
  ch_list_##X##_node *cur = l.head;                                            \
  while (cur != NULL)

#endif // _CH_MACRO_H
