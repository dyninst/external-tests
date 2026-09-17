// Tests BPatch analysis exclusion -- BPatch::addAnalysisExcludePattern and the
// on-demand parsing behind it -- across every path by which Dyninst can take on
// an object, and on ppc64 the Power ABI v2 TOC handling for an object that was
// excluded and so is parsed one function at a time.
//
//   exclusion_test              run all five scenarios, verbosely
//   exclusion_test <scenario>   run one scenario (this is what ctest does)
//
//   create       processCreate
//   attach       processAttach to an already-running process; also checks that
//                a pattern matching the Dyninst runtime library is ignored,
//                which matters here because under attach the runtime is
//                injected and so arrives by the dlopen path
//   dlopen       a library dlopen'd *after* attach, one matching the exclusion
//                pattern and one not
//   rewrite      openBinary with dependencies; instrument the executable
//   rewrite-dep  openBinary with dependencies; instrument a function inside the
//                excluded dependency
//
// create, attach and dlopen need ProcControl.  rewrite and rewrite-dep do not.
//
// Every scenario runs twice, once as a baseline and once with the library
// excluded, and both must reach the same answers.  For the rewriting scenarios
// the rewritten binary is also executed and its output must match.
//
// Each run happens in its own directory holding fresh copies of the libraries:
// rewriting with dependencies modifies dependency libraries IN PLACE, so runs
// sharing a directory corrupt one another and become order-dependent.
//
// The oracle is deliberately behavioural -- returned values -- because nothing
// this exercises is publicly observable: the TOC base lives on mapped_object
// and the Power entry-point links on func_instance.  libtoc.so holds only
// functions that reach global data, so on ppc64 every call into it needs a
// correct r2, and a call in from another object needs Dyninst to know that
// object's TOC base.  A wrong TOC base, or an unlinked global/local entry pair,
// shows up as a wrong return value rather than as a quiet difference.

#include "BPatch.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_image.h"
#include "BPatch_object.h"
#include "BPatch_module.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_snippet.h"

#include "CFG.h"
#include "CodeObject.h"
#include "Function.h"
#include "Symbol.h"
#include "Symtab.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef ARTIFACT_DIR
#define ARTIFACT_DIR "."
#endif

enum class Scenario
{
    Create,
    Attach,
    Dlopen,
    Rewrite,
    RewriteDep
};


// In the order they are run and reported.
static const std::vector<Scenario> &allScenarios()
{
    static const std::vector<Scenario> all = { Scenario::Create,
                                               Scenario::Attach,
                                               Scenario::Dlopen,
                                               Scenario::Rewrite,
                                               Scenario::RewriteDep };
    return all;
}


static const std::map<std::string, Scenario> &scenarioByName()
{
    static const std::map<std::string, Scenario> byName = {
        { "create",      Scenario::Create },
        { "attach",      Scenario::Attach },
        { "dlopen",      Scenario::Dlopen },
        { "rewrite",     Scenario::Rewrite },
        { "rewrite-dep", Scenario::RewriteDep }
    };
    return byName;
}


static std::string scenarioName(Scenario scenario)
{
    for (auto const &entry : scenarioByName())  {
        if (entry.second == scenario)
            return entry.first;
    }
    return "unknown";
}


// The names, in run order, for a usage message.
static std::string scenarioNames(const std::string &separator)
{
    std::string joined;
    for (auto const &scenario : allScenarios())  {
        if (!joined.empty())
            joined += separator;
        joined += scenarioName(scenario);
    }
    return joined;
}

static int failures = 0;

// The excluded libraries carry a couple of hundred branchy functions, well over
// a thousand basic blocks, so a real exclusion saves far more than this.  The
// margin is here so that a regression which excludes nothing cannot slip
// through on an incidental difference of a few blocks.
static const size_t kMinBlocksSaved = 500;

// Likewise for functions: the excluded libraries hold a little over two
// hundred each.
static const size_t kMinFunctionsSaved = 100;

// What is present is a property of the libraries, so excluding one must not
// change it -- except for the functions the parser invents for unsymbolised
// code and hands to the symbol table, which it finds fewer of when it parses
// less and which move between runs regardless.  Measured drift over repeated
// runs here is -1 to +3, so this leaves an order of magnitude of headroom
// while still failing if exclusion starts moving the count in earnest.
static const size_t kMaxTargDrift = 25;


// A by-hand run reports timings and the scale of what was analyzed; under
// ctest neither is wanted, since ctest reports its own timing and shows this
// output only when a test fails.
static bool showDurations = false;


static bool runningUnderCtest()
{
    return getenv("CTEST_INTERACTIVE_DEBUG_MODE") != NULL;
}


static double nowSeconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double) ts.tv_sec + (double) ts.tv_nsec / 1e9;
}


static double elapsedSeconds(double since)
{
    return nowSeconds() - since;
}


static void check(bool ok, const char *what)
{
    std::printf("    %-52s %s\n", what, ok ? "ok" : "FAIL");
    if (!ok)
        failures++;
}


static void checkVal(long got, long want, const char *what)
{
    bool ok = (got == want);
    std::printf("    %-52s %s", what, ok ? "ok" : "FAIL");
    if (!ok)
        std::printf("  (got %#lx, want %#lx)", got, want);
    std::printf("\n");
    if (!ok)
        failures++;
}


// ------------------------------------------------------------------ helpers


static BPatch_object *findObj(BPatch_image *img, const char *frag)
{
    std::vector<BPatch_object *> objs;
    img->getObjects(objs);
    for (size_t i = 0; i < objs.size(); i++)
        if (objs[i]->name().find(frag) != std::string::npos)
            return objs[i];
    return NULL;
}


// How many functions the object has a CFG for.  An excluded object reports
// none until something asks for one by name.
static size_t funcCount(BPatch_object *o)
{
    std::vector<BPatch_module *> mods;
    o->modules(mods);
    size_t n = 0;
    for (size_t m = 0; m < mods.size(); m++)  {
        std::vector<BPatch_function *> p;
        mods[m]->getProcedures(p, true);
        n += p.size();
    }
    return n;
}


// Reported, not asserted.  Dyninst's parse is not deterministic under OpenMP,
// in two unrelated ways, neither caused by exclusion -- a baseline run varies
// against itself just as much:
//
//   - functionsPresent is not a property of the file.  Parsing adds functions
//     to the symbol table for code it reaches with no symbol on it, named
//     targ<hex> by StandardParseData::createAndRecordFunc, and how many such
//     targets the parallel parser finds varies: libstdc++ gains six of them
//     single-threaded and eight with sixteen threads.  Discount those and the
//     count is identical every run, and equal to functionsAnalyzed.  Counting
//     unique addresses does not help, since these really are distinct
//     addresses rather than aliases of known ones.
//
//   - blocksAnalyzed moves by a few dozen, in libc, with the function count
//     unchanged.  The same functions come back with fewer basic blocks, so
//     that one really is less analysis.
//
// With OMP_NUM_THREADS=1 every number is stable.  The checks below therefore
// want a margin far larger than either wobble, and functionsPresent -- which
// ought to be identical on both sides -- is printed rather than compared.
// Where a function came from.  Anything Dyninst adds itself keeps the symbol
// index of -1 that Symtab::createFunction leaves, so a real ELF symbol table
// entry is the one with an index; of the rest, the parser names the ones it
// invented for unsymbolised code targ<hex>.
struct FunctionCounts
{
    size_t elfSymtab;
    size_t otherNonTarg;
    size_t otherTarg;
    size_t total;
};


struct AnalysisTotals
{
    FunctionCounts present;
    FunctionCounts analyzed;
    size_t blocksAnalyzed;
};


static bool isTargName(const std::string &name)
{
    if (name.size() <= 4 || name.compare(0, 4, "targ") != 0)
        return false;
    for (size_t i = 4; i < name.size(); i++)  {
        if (!isxdigit((unsigned char) name[i]))
            return false;
    }
    return true;
}


static bool fromElfSymtab(Dyninst::SymtabAPI::Function *func)
{
    if (func == NULL)
        return false;
    std::vector<Dyninst::SymtabAPI::Symbol *> syms;
    if (!func->getSymbols(syms))
        return false;
    for (auto *sym : syms)  {
        if (sym->getIndex() >= 0)
            return true;
    }
    return false;
}


static void countFunction(FunctionCounts &counts, bool elf, const std::string &name)
{
    counts.total++;
    if (elf)
        counts.elfSymtab++;
    else if (isTargName(name))
        counts.otherTarg++;
    else
        counts.otherNonTarg++;
}


// What Dyninst has analyzed, across every object, against what the symbol
// tables offer.  Reading a CodeObject's function list does not cause parsing,
// so this reports the work already done rather than provoking more.
static AnalysisTotals analysisTotals(BPatch_image *img)
{
    AnalysisTotals totals = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0 };
    std::vector<BPatch_object *> objs;
    img->getObjects(objs);

    for (auto *obj : objs)  {
        Dyninst::SymtabAPI::Symtab *sym = Dyninst::SymtabAPI::convert(obj);
        if (sym != NULL)  {
            for (auto *func : sym->getAllFunctionsRef())
                countFunction(totals.present, fromElfSymtab(func), func->getName());
        }

        Dyninst::ParseAPI::CodeObject *co = Dyninst::ParseAPI::convert(obj);
        if (co == NULL)
            continue;
        for (auto *func : co->funcs())  {
            // the parser works in the same offsets as the symbol table, so the
            // matching entry says whether this came from ELF
            Dyninst::SymtabAPI::Function *symFunc = NULL;
            bool elf = false;
            if (sym != NULL && sym->findFuncByEntryOffset(symFunc, func->addr()))
                elf = fromElfSymtab(symFunc);
            countFunction(totals.analyzed, elf, func->name());
            totals.blocksAnalyzed += func->num_blocks();
        }
    }
    return totals;
}


// The worker leaves its count here for the parent to compare.  The working
// directory is this run's own, so the two halves cannot collide.
static void recordAnalysisTotals(BPatch_image *img)
{
    AnalysisTotals totals = analysisTotals(img);
    std::ofstream out("totals.txt");
    out << totals.present.elfSymtab << " "
        << totals.present.otherNonTarg << " "
        << totals.present.otherTarg << " "
        << totals.present.total << " "
        << totals.analyzed.elfSymtab << " "
        << totals.analyzed.otherNonTarg << " "
        << totals.analyzed.otherTarg << " "
        << totals.analyzed.total << " "
        << totals.blocksAnalyzed << "\n";
}


static bool readAnalysisTotals(const std::string &dir, AnalysisTotals &totals)
{
    std::ifstream in((dir + "/totals.txt").c_str());
    if (!in)
        return false;

    std::vector<unsigned long> values(9, 0);
    for (size_t i = 0; i < values.size(); i++)
        in >> values[i];
    if (!in)
        return false;

    totals.present.elfSymtab = (size_t) values[0];
    totals.present.otherNonTarg = (size_t) values[1];
    totals.present.otherTarg = (size_t) values[2];
    totals.present.total = (size_t) values[3];
    totals.analyzed.elfSymtab = (size_t) values[4];
    totals.analyzed.otherNonTarg = (size_t) values[5];
    totals.analyzed.otherTarg = (size_t) values[6];
    totals.analyzed.total = (size_t) values[7];
    totals.blocksAnalyzed = (size_t) values[8];
    return true;
}


// One metric.  On the excluded side it also carries how far it moved from the
// baseline, signed either way so the direction is never in doubt.
static void reportMetric(const char *label, size_t value,
                         const size_t *baseline)
{
    if (baseline == NULL)  {
        std::printf("    %-52s %zu\n", label, value);
        return;
    }
    long delta = (long) value - (long) *baseline;
    std::printf("    %-52s %zu (delta: %+ld)\n", label, value, delta);
}


static void reportCounts(const char *what, const FunctionCounts &counts,
                         const FunctionCounts *baseline)
{
    std::string prefix(what);
    reportMetric((prefix + ": elf symtab").c_str(), counts.elfSymtab,
                 baseline ? &baseline->elfSymtab : NULL);
    reportMetric((prefix + ": other non-targ").c_str(), counts.otherNonTarg,
                 baseline ? &baseline->otherNonTarg : NULL);
    reportMetric((prefix + ": other targ").c_str(), counts.otherTarg,
                 baseline ? &baseline->otherTarg : NULL);
    reportMetric((prefix + ": total").c_str(), counts.total,
                 baseline ? &baseline->total : NULL);
}


static void reportTotals(const AnalysisTotals &totals,
                         const AnalysisTotals *baseline)
{
    reportCounts("functions present", totals.present,
                 baseline ? &baseline->present : NULL);
    reportCounts("functions analyzed", totals.analyzed,
                 baseline ? &baseline->analyzed : NULL);
    reportMetric("blocks analyzed", totals.blocksAnalyzed,
                 baseline ? &baseline->blocksAnalyzed : NULL);
}


static BPatch_function *findOne(BPatch_image *img, const char *name)
{
    std::vector<BPatch_function *> f;
    img->findFunction(name, f);
    return f.empty() ? NULL : f[0];
}


// A call into the object under test.  On ppc64 this needs that object's TOC
// base in order to set r2.
static void callAndCheck(BPatch_process *proc, BPatch_image *img,
                         const char *fname, long want, const char *what)
{
    BPatch_function *f = findOne(img, fname);
    if (!f)  {
        check(false, what);
        return;
    }
    std::vector<BPatch_snippet *> noArgs;
    void *r = proc->oneTimeCode(BPatch_funcCallExpr(*f, noArgs));
    checkVal((long)(intptr_t)r, want, what);
}


static pid_t spawn(const char *path, const char *mode)
{
    pid_t pid = fork();
    if (pid == 0)  {
        execl(path, path, mode, (char *)NULL);
        _exit(127);
    }
    return pid;
}


// ---------------------------------------------------------------- scenarios


static int sceneCreate(BPatch &bpatch, bool exclude)
{
    const char *av[] = { "./mutatee", NULL };
    BPatch_process *proc = bpatch.processCreate("./mutatee", av);
    if (!proc)  {
        std::printf("    processCreate FAILED\n");
        return 1;
    }
    BPatch_image *img = proc->getImage();

    BPatch_object *lib = findObj(img, "libtoc");
    check(lib != NULL, "excluded library still present as an object");
    if (!lib)
        return 1;

    size_t n = funcCount(lib);
    check(exclude ? (n == 0) : (n > 0),
          exclude ? "no CFG built up front" : "library parsed up front");

    // Here the runtime library is LD_PRELOADed, so it comes in with the
    // initial objects rather than by dlopen as it does on attach.  That is a
    // second way into the guard that keeps it from ever being excluded.
    BPatch_object *rt = findObj(img, "dyninstAPI_RT");
    check(rt != NULL, "runtime library present (preloaded at startup)");
    if (rt != NULL)
        check(funcCount(rt) > 0, "runtime analyzed despite matching pattern");

    check(findOne(img, "toc_get") != NULL, "toc_get found by name");
    callAndCheck(proc, img, "toc_get", 0x5EED, "call toc_get() -- global via TOC");
    callAndCheck(proc, img, "toc_sum", 110,    "call toc_sum() -- table via TOC");

    // Instrumenting a function inside the library relocates it, which on ppc64
    // needs its global/local entry pair linked or it is relocated twice.
    BPatch_function *marker = findOne(img, "toc_marker");
    check(marker != NULL, "toc_marker found in the library");
    if (marker)  {
        std::vector<BPatch_point *> *pts = marker->findPoint(BPatch_entry);
        check(pts && !pts->empty(), "entry point inside the library");
        if (pts && !pts->empty())  {
            // Deliberately a plain call: allocating a variable would need
            // findType(), and that forces DWARF type parsing, which is not
            // what this test is about.
            BPatch_function *tg = findOne(img, "toc_get");
            if (tg)  {
                std::vector<BPatch_snippet *> noArgs;
                check(proc->insertSnippet(BPatch_funcCallExpr(*tg, noArgs), *pts) != NULL,
                      "instrumented a function in the library");
            }
        }
    }

    recordAnalysisTotals(img);

    proc->continueExecution();
    while (!proc->isTerminated())
        bpatch.waitForStatusChange();
    check(true, "mutatee ran to completion");
    return 0;
}


static int sceneAttach(BPatch &bpatch, bool exclude)
{
    pid_t pid = spawn("./mutatee", "wait");
    usleep(400000);

    BPatch_process *proc = bpatch.processAttach(NULL, pid);
    if (!proc)  {
        std::printf("    processAttach FAILED\n");
        kill(pid, SIGKILL);
        return 1;
    }
    check(true, "attached to a running process");
    BPatch_image *img = proc->getImage();

    BPatch_object *lib = findObj(img, "libtoc");
    BPatch_object *rt = findObj(img, "dyninstAPI_RT");
    check(lib != NULL, "library present");
    check(rt != NULL, "runtime library present (injected on attach)");

    if (lib)  {
        size_t n = funcCount(lib);
        check(exclude ? (n == 0) : (n > 0),
              exclude ? "no CFG built up front" : "library parsed up front");
    }
    // Dyninst's own runtime is never excluded, even when a pattern matches it.
    if (rt) check(funcCount(rt)
        > 0, "runtime analyzed despite matching pattern");

    check(findOne(img, "toc_get") != NULL, "toc_get found by name");
    callAndCheck(proc, img, "toc_get", 0x5EED, "call toc_get() -- global via TOC");

    recordAnalysisTotals(img);

    proc->detach(true);
    kill(pid, SIGKILL);
    int st;
    waitpid(pid, &st, 0);
    return 0;
}


static int sceneDlopen(BPatch &bpatch, bool exclude)
{
    pid_t pid = spawn("./mutatee", "dlopen");
    usleep(400000);

    BPatch_process *proc = bpatch.processAttach(NULL, pid);
    if (!proc)  {
        std::printf("    processAttach FAILED\n");
        kill(pid, SIGKILL);
        return 1;
    }
    BPatch_image *img = proc->getImage();

    check(findObj(img, "libplugA") == NULL, "plugins not yet loaded at attach time");

    proc->continueExecution();
    for (int i = 0; i < 300 && !(findObj(img, "libplugA") && findObj(img, "libplugB")); i++)  {
        bpatch.pollForStatusChange();
        usleep(50000);
    }
    proc->stopExecution();

    BPatch_object *A = findObj(img, "libplugA");
    BPatch_object *B = findObj(img, "libplugB");
    check(A != NULL, "libplugA seen after dlopen");
    check(B != NULL, "libplugB seen after dlopen");
    if (!A || !B)  {
        kill(pid, SIGKILL);
        return 1;
    }

    size_t nA = funcCount(A);
    check(exclude ? (nA == 0) : (nA > 0),
          exclude ? "dlopen'd + matches pattern -> no CFG"
                  : "dlopen'd + no pattern      -> parsed");
    check(funcCount(B) > 0, "dlopen'd + does not match pattern -> parsed");

    check(findOne(img, "plugA_one") != NULL, "plugA_one found by name");
    check(findOne(img, "plugB_one") != NULL, "plugB_one found by name");
    callAndCheck(proc, img, "plugA_one", 0xAAAA, "call plugA_one()");
    callAndCheck(proc, img, "plugB_one", 0xBBBB, "call plugB_one()");

    recordAnalysisTotals(img);

    proc->detach(true);
    kill(pid, SIGKILL);
    int st;
    waitpid(pid, &st, 0);
    return 0;
}


static int sceneRewrite(BPatch &bpatch, bool exclude, bool intoDep)
{
    BPatch_binaryEdit *be = bpatch.openBinary("./mutatee", true /* dependencies */);
    if (!be)  {
        std::printf("    openBinary FAILED\n");
        return 1;
    }
    BPatch_image *img = be->getImage();

    std::vector<BPatch_object *> objs;
    img->getObjects(objs);
    BPatch_object *lib = findObj(img, "libtoc");
    check(lib != NULL, "dependency opened");
    check(objs.size() > 2, "dependency discovery continued past it");
    if (!lib)
        return 1;

    size_t n = funcCount(lib);
    check(exclude ? (n == 0) : (n > 0),
          exclude ? "excluded dependency has no CFG up front"
                  : "dependency parsed up front");

    BPatch_function *target = intoDep ? findOne(img, "toc_get") : findOne(img, "main");
    check(target != NULL, intoDep ? "toc_get found (inside the dependency)"
                                  : "main found (in the executable)");
    if (!target)
        return 1;

    std::vector<BPatch_point *> *pts = target->findPoint(BPatch_entry);
    check(pts && !pts->empty(), "entry point found");
    if (!pts || pts->empty())
        return 1;

    BPatch_function *pf = findOne(img, "printf");
    check(pf != NULL, "printf found for the snippet");
    if (!pf)
        return 1;

    std::vector<BPatch_snippet *> args;
    args.push_back(new BPatch_constExpr(intoDep ? "instrumented: dependency\n"
                                                : "instrumented: executable\n"));
    check(be->insertSnippet(BPatch_funcCallExpr(*pf, args), *pts) != NULL,
          intoDep ? "snippet inserted into the dependency"
                  : "snippet inserted into the executable");

    check(be->writeFile("out/mutatee"), "writeFile produced the rewritten binary");
    recordAnalysisTotals(img);
    return 0;
}


// The worker: one scenario, one mode, in an already-prepared directory.
static int runWorker(Scenario scenario, bool exclude)
{
    BPatch bpatch;
    if (exclude)  {
        bpatch.addAnalysisExcludePattern("libtoc.so*");
        bpatch.addAnalysisExcludePattern("libplugA.so*");
        // must be ignored: Dyninst's own runtime is never excluded
        bpatch.addAnalysisExcludePattern("libdyninstAPI_RT*");
    }

    int rc = 0;
    switch (scenario)  {
    case Scenario::Create:
        rc = sceneCreate(bpatch, exclude);
        break;
    case Scenario::Attach:
        rc = sceneAttach(bpatch, exclude);
        break;
    case Scenario::Dlopen:
        rc = sceneDlopen(bpatch, exclude);
        break;
    case Scenario::Rewrite:
        rc = sceneRewrite(bpatch, exclude, false);
        break;
    case Scenario::RewriteDep:
        rc = sceneRewrite(bpatch, exclude, true);
        break;
    }

    return (rc || failures) ? 1 : 0;
}


// ------------------------------------------------------------- orchestration


// PCProcess::setEnvPreload finds the runtime library through the library search
// path rather than through DYNINSTAPI_RT_LIB, so processCreate fails unless the
// directory holding it is on LD_LIBRARY_PATH.  Put it there ourselves, so that
// running this by hand needs nothing but DYNINSTAPI_RT_LIB, and so that ctest
// need only set that one variable.  Prepended, so an existing setting survives.
// Each run happens with the working directory changed, and the worker is this
// same binary re-executed, so a relative argv[0] would no longer resolve.
static std::string selfPath(const char *argv0)
{
    char buf[4096];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n > 0)  {
        buf[n] = '\0';
        return std::string(buf);
    }
    char *r = realpath(argv0, NULL);
    if (r)  {
        std::string s(r);
        free(r);
        return s;
    }
    return std::string(argv0);
}


static void ensureRuntimeOnLibraryPath()
{
    const char *rt = getenv("DYNINSTAPI_RT_LIB");
    if (!rt)
        return;
    std::string dir(rt);
    size_t slash = dir.rfind('/');
    if (slash == std::string::npos)
        return;
    dir.erase(slash);

    const char *old = getenv("LD_LIBRARY_PATH");
    if (old && *old)  {
        std::string cur(old);
        // already there?
        if (cur == dir || cur.compare(0, dir.size() + 1, dir + ":") == 0)
            return;
        dir += std::string(":") + cur;
    }
    setenv("LD_LIBRARY_PATH", dir.c_str(), 1);
}


static bool copyFile(const std::string &from, const std::string &to)
{
    std::ifstream in(from.c_str(), std::ios::binary);
    if (!in)
        return false;
    std::ofstream out(to.c_str(), std::ios::binary | std::ios::trunc);
    if (!out)
        return false;
    out << in.rdbuf();
    out.close();
    struct stat st;
    if (stat(from.c_str(), &st) == 0)
        chmod(to.c_str(), st.st_mode);
    return true;
}


// A fresh directory per run: rewriting modifies dependency libraries in place.
static bool prepareDir(const std::string &dir)
{
    std::string rm = "rm -rf '" + dir + "'";
    if (std::system(rm.c_str()) != 0)
        return false;
    if (mkdir(dir.c_str(), 0755) != 0)
        return false;
    if (mkdir((dir + "/out").c_str(), 0755) != 0)
        return false;

    const char *artifacts[] = { "libtoc.so", "libplugA.so", "libplugB.so", "mutatee" };
    for (int i = 0; i < 4; i++)  {
        if (!copyFile(std::string(ARTIFACT_DIR) + "/" + artifacts[i],
                     dir + "/" + artifacts[i]))
            return false;
    }
    return true;
}


// Run one (scenario, mode) as a child, in its own directory.  Isolating each in
// a process keeps one run's Dyninst state from reaching the next.
static int runChild(const std::string &self, Scenario scenario, bool exclude,
                    const std::string &dir)
{
    std::string name = scenarioName(scenario);

    pid_t pid = fork();
    if (pid == 0)  {
        if (chdir(dir.c_str()) != 0)
            _exit(2);
        execl(self.c_str(), self.c_str(), "--worker", name.c_str(),
              exclude ? "exclude" : "baseline", (char *)NULL);
        _exit(127);
    }
    int st = 0;
    waitpid(pid, &st, 0);
    return WIFEXITED(st) ? WEXITSTATUS(st) : 1;
}


// Execute a rewritten binary and capture what it printed.
static std::string runRewritten(const std::string &dir)
{
    std::string exe = dir + "/out/mutatee";
    struct stat st;
    if (stat(exe.c_str(), &st) != 0)
        return "";

    std::string out = dir + "/ran.txt";
    pid_t pid = fork();
    if (pid == 0)  {
        if (chdir(dir.c_str()) != 0)
            _exit(2);
        int fd = open("ran.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd >= 0)  {
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }
        // "." first so the rewritten dependency beside us is the one loaded,
        // then wherever the Dyninst runtime library lives.
        std::string ld = ".";
        const char *rt = getenv("DYNINSTAPI_RT_LIB");
        if (rt)  {
            std::string d(rt);
            size_t slash = d.rfind('/');
            if (slash != std::string::npos)
                ld += ":" + d.substr(0, slash);
        }
        const char *oldLd = getenv("LD_LIBRARY_PATH");
        if (oldLd && *oldLd) ld += std::string(":")
            + oldLd;
        setenv("LD_LIBRARY_PATH", ld.c_str(), 1);
        execl("./out/mutatee", "./out/mutatee", (char *)NULL);
        _exit(127);
    }
    int status = 0;
    waitpid(pid, &status, 0);

    std::ifstream in(out.c_str());
    std::string all;
    std::string line;
    while (std::getline(in, line))  {
        // the mutatee indents itself to line up in this log; undo that here,
        // where the text is quoted onto a single line
        size_t text = line.find_first_not_of(' ');
        if (text != std::string::npos)
            line.erase(0, text);
        all += line + "\n";
    }
    return all;
}


// A failing comparison has to carry its own numbers.  The figures above are
// printed only when a person is watching, so without this a failure under
// ctest says what broke but never by how much.
static void checkCount(bool ok, const char *what, size_t baseline, size_t excluded)
{
    std::printf("    %-52s %s", what, ok ? "ok" : "FAIL");
    if (!ok)  {
        long delta = (long) excluded - (long) baseline;
        std::printf("  (baseline %zu, excluded %zu, delta %+ld)",
                    baseline, excluded, delta);
    }
    std::printf("\n");
}


// One scenario: baseline and excluded, then compare what each produced.
static int runScenario(const std::string &self, Scenario scenario)
{
    double started = nowSeconds();
    std::string name = scenarioName(scenario);

    std::printf("=== %s ===\n", name.c_str());
    int bad = 0;
    std::vector<std::string> ran(2);
    std::vector<AnalysisTotals> totals(2);
    std::vector<bool> haveTotals(2, false);

    // Only the rewriting scenarios leave a binary behind to re-run.  The
    // snippet must actually have run in each of them: comparing the two runs
    // alone is not enough, because if the rewritten binary loaded the original
    // library then both sides agree and prove nothing.
    bool rewriting = (scenario == Scenario::Rewrite ||
                      scenario == Scenario::RewriteDep);
    std::string marker = (scenario == Scenario::RewriteDep)
                             ? "instrumented: dependency"
                             : "instrumented: executable";

    for (int mode = 0; mode < 2; mode++)  {
        double modeStarted = nowSeconds();
        bool exclude = (mode == 1);
        std::string dir = name + (exclude ? ".excluded" : ".baseline");
        std::printf("  -- %s --\n", exclude ? "excluded" : "baseline");

        if (!prepareDir(dir))  {
            std::printf("    could not prepare '%s'\n", dir.c_str());
            return 1;
        }
        if (runChild(self, scenario, exclude, dir) != 0)
            bad = 1;
        ran[mode] = runRewritten(dir);
        if (!ran[mode].empty())  {
            std::string one = ran[mode];
            for (size_t i = 0; i < one.size(); i++)
                if (one[i] == '\n')
                    one[i] = '|';
            std::printf("    rewritten binary output: %s\n", one.c_str());
        }
        if (rewriting)  {
            bool present = ran[mode].find(marker) != std::string::npos;
            std::printf("    %-52s %s\n", "instrumentation actually ran",
                        present ? "ok" : "FAIL");
            if (!present)
                bad = 1;
        }

        haveTotals[mode] = readAnalysisTotals(dir, totals[mode]);
        if (showDurations && haveTotals[mode])  {
            // the excluded half is measured against the baseline just above it
            bool haveReference = exclude && haveTotals[0];
            reportTotals(totals[mode], haveReference ? &totals[0] : NULL);
        }
        if (showDurations)  {
            std::printf("    %-52s %.3f sec\n", "duration",
                        elapsedSeconds(modeStarted));
        }
    }

    if (rewriting)  {
        // A rewritten binary must also behave the same either way.  This needs
        // no knowledge of what Dyninst did internally.
        bool same = (ran[0] == ran[1]);
        std::printf("    %-52s %s\n",
                    "rewritten output identical baseline vs excluded",
                    same ? "ok" : "FAIL");
        if (!same)
            bad = 1;
    }

    // The point of the feature: excluding an object must leave Dyninst with
    // less to analyze, measured over every object rather than just the one.
    if (haveTotals[0] && haveTotals[1])  {
        const AnalysisTotals &base = totals[0];
        const AnalysisTotals &excl = totals[1];
        size_t functionsSaved = (excl.analyzed.total < base.analyzed.total)
                                    ? base.analyzed.total - excl.analyzed.total
                                    : 0;
        size_t blocksSaved = (excl.blocksAnalyzed < base.blocksAnalyzed)
                                 ? base.blocksAnalyzed - excl.blocksAnalyzed
                                 : 0;

        // Present is what the libraries hold: the counts that do not come
        // from parsing have to be identical on both sides.
        bool sameElf = (base.present.elfSymtab == excl.present.elfSymtab);
        checkCount(sameElf, "same elf symtab functions present",
                   base.present.elfSymtab, excl.present.elfSymtab);
        if (!sameElf)
            bad = 1;

        bool sameOther = (base.present.otherNonTarg == excl.present.otherNonTarg);
        checkCount(sameOther, "same other non-targ functions present",
                   base.present.otherNonTarg, excl.present.otherNonTarg);
        if (!sameOther)
            bad = 1;

        size_t targDrift = (base.present.otherTarg > excl.present.otherTarg)
                               ? base.present.otherTarg - excl.present.otherTarg
                               : excl.present.otherTarg - base.present.otherTarg;
        bool targClose = targDrift <= kMaxTargDrift;
        checkCount(targClose, "targ functions present within threshold",
                   base.present.otherTarg, excl.present.otherTarg);
        if (!targClose)
            bad = 1;

        bool fewerFunctions = functionsSaved >= kMinFunctionsSaved;
        checkCount(fewerFunctions, "materially fewer functions when excluded",
                   base.analyzed.total, excl.analyzed.total);
        if (!fewerFunctions)
            bad = 1;

        bool fewerBlocks = blocksSaved >= kMinBlocksSaved;
        checkCount(fewerBlocks, "materially fewer blocks when excluded",
                   base.blocksAnalyzed, excl.blocksAnalyzed);
        if (!fewerBlocks)
            bad = 1;
    }

    if (showDurations)  {
        std::printf("=== %s: %s (%.3f sec) ===\n\n", name.c_str(),
                    bad ? "FAILED" : "passed", elapsedSeconds(started));
    }  else  {
        std::printf("=== %s: %s ===\n\n", name.c_str(),
                    bad ? "FAILED" : "passed");
    }
    return bad;
}


// Dyninst finds its runtime library through DYNINSTAPI_RT_LIB (note the API
// in the middle).  Without it every scenario fails deep inside Dyninst with
// several lines of FATAL, which says little about what to do.  Check it here
// and say so once.
static bool runtimeLibraryUsable()
{
    const char *rt = getenv("DYNINSTAPI_RT_LIB");
    if (rt == NULL || *rt == '\0')  {
        std::fprintf(stderr,
                     "error: DYNINSTAPI_RT_LIB is not set\n"
                     "       set it to the full path of libdyninstAPI_RT.so, "
                     "e.g.\n"
                     "         DYNINSTAPI_RT_LIB=<prefix>/lib64/libdyninstAPI_RT.so\n");
        return false;
    }
    if (access(rt, R_OK) != 0)  {
        std::fprintf(stderr,
                     "error: DYNINSTAPI_RT_LIB names a file that cannot be read:\n"
                     "         %s\n", rt);
        return false;
    }
    return true;
}


int main(int argc, char **argv)
{
    if (!runtimeLibraryUsable())
        return 2;
    ensureRuntimeOnLibraryPath();

    // internal: one scenario, one mode, cwd already prepared
    if (argc >= 4 && std::string(argv[1]) == "--worker")  {
        auto found = scenarioByName().find(argv[2]);
        if (found == scenarioByName().end())  {
            std::fprintf(stderr, "unknown scenario '%s'\n", argv[2]);
            return 2;
        }
        return runWorker(found->second, std::string(argv[3]) == "exclude");
    }

    if (argc > 2)  {
        std::fprintf(stderr, "usage: %s [%s]\n", argv[0],
                     scenarioNames("|").c_str());
        std::fprintf(stderr, "       with no scenario, all of them run\n");
        return 2;
    }

    std::string self = selfPath(argv[0]);

    // report timings and scale unless ctest is driving us
    showDurations = !runningUnderCtest();
    double started = nowSeconds();

    int bad = 0;
    if (argc == 2)  {
        auto found = scenarioByName().find(argv[1]);
        if (found == scenarioByName().end())  {
            std::fprintf(stderr, "unknown scenario '%s'\n", argv[1]);
            std::fprintf(stderr, "       expected one of: %s\n",
                         scenarioNames(", ").c_str());
            return 2;
        }
        bad = runScenario(self, found->second);
    }  else  {
        // every scenario, verbosely
        for (auto const &scenario : allScenarios())
            if (runScenario(self, scenario) != 0)
                bad = 1;
        std::printf("%s (%.3f sec)\n", bad ? "FAILURES" : "ALL PASS",
                    elapsedSeconds(started));
    }
    return bad;
}
