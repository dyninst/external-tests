// Characterises how a function the parser discovered by following control flow
// gets its name.
//
//   discovered_names_test              run both scenarios, verbosely
//   discovered_names_test <scenario>   run one scenario (this is what ctest does)
//
//   ordinary  libasm.so parsed as usual
//   excluded  libasm.so excluded from analysis, so asm_entry and everything it
//             reaches is parsed on demand, with no hints at all
//
// libasm.so is hand-written assembly.  asm_entry is typed, so a parse gets a
// hint for it; the two functions it calls are not.  One carries a plain label,
// which the assembler emits as an untyped symbol -- the name its author gave
// it, which Symtab does not aggregate into a Function and which therefore
// never becomes a hint.  The other carries no label at all.
//
// In an excluded object there are no hints, so both arrive at the parser
// unnamed and the untyped symbol is the only name the first one has.  Taking
// it there is what makes an excluded object report what a full parse would.
//
// An ordinary parse must be unaffected.  It arrives with its hints applied,
// and what it has not named by then it has decided not to name; renaming there
// would change what every existing caller sees, on every object, whether or
// not anything was ever excluded.

#include "BPatch.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_image.h"
#include "BPatch_function.h"
#include "BPatch_point.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef ARTIFACT_DIR
#define ARTIFACT_DIR "."
#endif


enum class Scenario
{
    Ordinary,
    Excluded
};


static const std::vector<Scenario> &allScenarios()
{
    static const std::vector<Scenario> all = { Scenario::Ordinary, Scenario::Excluded };
    return all;
}


static const std::map<std::string, Scenario> &scenarioByName()
{
    static const std::map<std::string, Scenario> byName = {
        { "ordinary", Scenario::Ordinary },
        { "excluded", Scenario::Excluded }
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


// Dyninst resolves a dependency against the current directory and
// LD_LIBRARY_PATH, among others, and does not read the target's DT_RPATH, so
// the test says where its own artifacts are rather than relying on where it
// was started from.
static void putArtifactDirOnLibraryPath()
{
    std::string path = ARTIFACT_DIR;
    const char *existing = getenv("LD_LIBRARY_PATH");
    if (existing != NULL && *existing != '\0')  {
        path += ":";
        path += existing;
    }
    setenv("LD_LIBRARY_PATH", path.c_str(), 1);
}


static int failures = 0;


static void check(bool ok, const std::string &what)
{
    if (ok)  {
        std::printf("    ok    %s\n", what.c_str());
    }  else  {
        std::printf("    FAIL  %s\n", what.c_str());
        failures++;
    }
}


static bool isInvented(const std::string &name)
{
    return name.compare(0, 4, "targ") == 0;
}


// The names asm_entry's two call sites report for their callees.
static std::vector<std::string> calleeNames(BPatch_function *entry)
{
    std::vector<std::string> names;
    std::vector<BPatch_point *> *points = entry->findPoint(BPatch_subroutine);
    if (points == NULL)
        return names;

    for (auto *point : *points)  {
        BPatch_function *callee = point->getCalledFunction();
        if (callee == NULL)  {
            names.push_back("(none)");
            continue;
        }
        char buf[256];
        callee->getName(buf, sizeof buf);
        names.push_back(buf);
    }
    return names;
}


static int count(const std::vector<std::string> &names, const std::string &want)
{
    int n = 0;
    for (auto const &name : names)  {
        if (name == want)
            n++;
    }
    return n;
}


static int countInvented(const std::vector<std::string> &names)
{
    int n = 0;
    for (auto const &name : names)  {
        if (isInvented(name))
            n++;
    }
    return n;
}


static int runScenario(Scenario scenario)
{
    bool excluded = scenario == Scenario::Excluded;
    std::printf("-- %s\n", scenarioName(scenario).c_str());

    int before = failures;

    BPatch bpatch;
    if (excluded)
        bpatch.addAnalysisExcludePattern("libasm.so*");

    std::string mutatee = std::string(ARTIFACT_DIR) + "/mutatee";
    BPatch_binaryEdit *be = bpatch.openBinary(mutatee.c_str(), true);
    if (be == NULL)  {
        std::fprintf(stderr, "openBinary(%s) failed\n", mutatee.c_str());
        return 2;
    }

    // Looking asm_entry up by name is what parses it, and everything it
    // reaches, when libasm.so was excluded.
    BPatch_image *img = be->getImage();
    std::vector<BPatch_function *> entries;
    img->findFunction("asm_entry", entries, false);
    if (entries.size() != 1)  {
        std::fprintf(stderr, "findFunction(\"asm_entry\") returned %zu, expected 1\n",
                     entries.size());
        return 2;
    }

    std::vector<std::string> callees = calleeNames(entries[0]);
    std::printf("  asm_entry's callees ->");
    for (auto const &name : callees)
        std::printf(" %s", name.c_str());
    std::printf("\n");

    check(callees.size() == 2, "asm_entry has two call sites");

    if (excluded)  {
        check(count(callees, "labelled_helper") == 1,
              "excluded: the labelled callee reports the name its symbol gives");
        check(countInvented(callees) == 1,
              "excluded: the unlabelled callee keeps an invented name");
    }  else  {
        check(count(callees, "labelled_helper") == 0,
              "ordinary: naming is unchanged, the labelled callee is not renamed");
        check(countInvented(callees) == 2,
              "ordinary: both callees keep invented names");
    }

    return failures == before ? 0 : 1;
}


// Each scenario needs a process of its own.  Dyninst consults the first BPatch
// ever constructed -- BPatch::bpatch is set once and never moved -- while the
// exclusion patterns belong to the object they were added to, so patterns set
// on a second BPatch in the same process are never seen.  ctest runs one
// scenario per process anyway; this is for running the test by hand.
static int runScenarioInChild(const char *self, Scenario scenario)
{
    pid_t pid = fork();
    if (pid < 0)  {
        std::fprintf(stderr, "fork failed\n");
        return 2;
    }
    if (pid == 0)  {
        std::string name = scenarioName(scenario);
        const char *args[] = { self, name.c_str(), NULL };
        execv(self, const_cast<char *const *>(args));
        std::fprintf(stderr, "exec of %s failed\n", self);
        _exit(2);
    }
    int status = 0;
    if (waitpid(pid, &status, 0) < 0 || !WIFEXITED(status))
        return 2;
    return WEXITSTATUS(status);
}


int main(int argc, char **argv)
{
    putArtifactDirOnLibraryPath();

    if (argc > 2)  {
        std::fprintf(stderr, "usage: %s [%s]\n", argv[0],
                     scenarioNames("|").c_str());
        std::fprintf(stderr, "       with no scenario, both of them run\n");
        return 2;
    }

    if (argc == 2)  {
        auto found = scenarioByName().find(argv[1]);
        if (found == scenarioByName().end())  {
            std::fprintf(stderr, "unknown scenario '%s'\n", argv[1]);
            std::fprintf(stderr, "       expected one of: %s\n",
                         scenarioNames(", ").c_str());
            return 2;
        }
        return runScenario(found->second);
    }

    std::string self = argv[0];
    if (self.find('/') == std::string::npos)
        self = "./" + self;

    int bad = 0;
    for (auto const &scenario : allScenarios())  {
        if (runScenarioInChild(self.c_str(), scenario) != 0)
            bad = 1;
    }
    std::printf(bad ? "FAILURES\n" : "ALL PASS\n");
    return bad;
}
