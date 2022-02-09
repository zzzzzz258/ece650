#define POTATO_TRACESIZE 1000
struct _potato {
  int left_hops;
  int trace[POTATO_TRACESIZE];
};
typedef struct _potato potato;
