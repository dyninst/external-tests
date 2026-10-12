// The definition the mutatee imports.  Because it lives here and not in the
// executable, a call to it from the executable goes through a PLT stub, which
// is what these tests are about.

int plt_target(int x)
{
    return x + 1;
}
