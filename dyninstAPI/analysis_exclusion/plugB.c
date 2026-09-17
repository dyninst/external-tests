/* dlopen'd after attach; does NOT match the exclusion pattern */
int plugB_g = 0xBBBB;
int plugB_one(void)
{
    return plugB_g;
}
int plugB_two(void)
{
    return plugB_g + 1;
}
int plugB_three(void)
{
    return plugB_g + 2;
}
