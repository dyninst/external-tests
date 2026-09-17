/* dlopen'd after attach; matches the exclusion pattern */
int plugA_g = 0xAAAA;
int plugA_one(void)
{
    return plugA_g;
}
int plugA_two(void)
{
    return plugA_g + 1;
}
int plugA_three(void)
{
    return plugA_g + 2;
}

/* Bulk, so that excluding this library removes a measurable number of basic
   blocks rather than a handful.  Each function below is several blocks, and
   there are two hundred of them. */
#define PLUGA_FILLER(n)                                                         \
    int plugA_filler_##n(int x)                                                 \
    {                                                                         \
        int r = plugA_g;                                                   \
        if (x & 1)                                                            \
            r += plugA_g;                                                \
        else                                                                  \
            r -= plugA_g;                                                \
        for (int i = 0; i < (x & 7); i++)                                     \
            r += plugA_g;                                            \
        if (r > 100)                                                          \
            r = 100;                                                          \
        return r;                                                             \
    }

#define PLUGA_FILLER_10(p)                                                      \
    PLUGA_FILLER(p##0) PLUGA_FILLER(p##1) PLUGA_FILLER(p##2) PLUGA_FILLER(p##3)       \
    PLUGA_FILLER(p##4) PLUGA_FILLER(p##5) PLUGA_FILLER(p##6) PLUGA_FILLER(p##7)       \
    PLUGA_FILLER(p##8) PLUGA_FILLER(p##9)

#define PLUGA_FILLER_100(p)                                                     \
    PLUGA_FILLER_10(p##0) PLUGA_FILLER_10(p##1) PLUGA_FILLER_10(p##2)               \
    PLUGA_FILLER_10(p##3) PLUGA_FILLER_10(p##4) PLUGA_FILLER_10(p##5)               \
    PLUGA_FILLER_10(p##6) PLUGA_FILLER_10(p##7) PLUGA_FILLER_10(p##8)               \
    PLUGA_FILLER_10(p##9)

PLUGA_FILLER_100(1)
PLUGA_FILLER_100(2)
