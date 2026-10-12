// Characterises which functions a by-name lookup reports when the name is one
// the object imports rather than one it defines.
//
//   plt_stub_test              run both scenarios, verbosely
//   plt_stub_test <scenario>   run one scenario (this is what ctest does)
//
//   no-deps  openBinary without dependencies: only the executable is present,
//            so the stub is the only thing bearing the name
//   deps     openBinary with dependencies: the stub and the definition both
//            bear it, in different objects
//
// An external linkage stub -- a PLT entry on ELF -- forwards to a definition
// another object supplies.  It is not instrumentable, so it is reported only
// when the caller passes incUninstrumentable.  That is what the manual means
// by "will include uninstrumentable functions", and it is the one thing here
// that is easy to change by accident, because the stub lives in a table of its
// own rather than in the Symtab the lookup walks.
//
// Nothing is rewritten: the binaries are opened and queried, never written, so
// these scenarios need neither ProcControl nor a private copy of the artifacts.
//
// Functions are identified by the object that holds them, never by name.  A
// library function reports a different alias from run to run -- printf,
// __printf and _IO_printf are all the same address -- so a name is not a
// stable oracle even when it looks like one here.  The executable does not
// define plt_target, so anything reported for that name out of the executable
// is a stub by construction, and nothing has to be asked whether it is one.

#include "BPatch.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_image.h"
#include "BPatch_object.h"
#include "BPatch_module.h"
#include "BPatch_function.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <unistd.h>

#ifndef ARTIFACT_DIR
#define ARTIFACT_DIR "."
#endif


enum class Scenario
{
    NoDeps,
    Deps
};


static const std::vector<Scenario> &allScenarios()
{
    static const std::vector<Scenario> all = { Scenario::NoDeps, Scenario::Deps };
    return all;
}


static const std::map<std::string, Scenario> &scenarioByName()
{
    static const std::map<std::string, Scenario> byName = {
        { "no-deps", Scenario::NoDeps },
        { "deps",    Scenario::Deps }
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


// Dyninst resolves a dependency by trying the DT_NEEDED name against the
// current directory, then DYNINST_REWRITER_PATHS, LD_LIBRARY_PATH, the
// compiler's search directories and the ldconfig cache; see
// BinaryEdit::getResolvedLibraryPath.  It does not read the target's own
// DT_RPATH, so the $ORIGIN that lets the mutatee run from its build directory
// does nothing here.  Without this the deps scenario finds libdef.so only when
// the test is run from the directory holding it, which is how ctest runs it
// and is not how anyone runs it by hand.
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


// What a lookup found, described by where it lives rather than what it is
// called.
struct Found
{
    std::string object;   // base name of the owning object
    bool instrumentable = false;
};


static std::string baseName(const std::string &path)
{
    size_t slash = path.rfind('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}


static Found describe(BPatch_function *func)
{
    Found found;
    found.instrumentable = func->isInstrumentable();

    BPatch_module *mod = func->getModule();
    BPatch_object *obj = mod ? mod->getObject() : NULL;
    if (obj != NULL)
        found.object = baseName(obj->pathName());

    return found;
}


static std::vector<Found> describeAll(const std::vector<BPatch_function *> &funcs)
{
    std::vector<Found> all;
    for (auto *func : funcs)
        all.push_back(describe(func));
    return all;
}


// How many of these came from an object whose name contains `what`.
static int countFrom(const std::vector<Found> &found, const std::string &what)
{
    int n = 0;
    for (auto const &f : found)  {
        if (f.object.find(what) != std::string::npos)
            n++;
    }
    return n;
}


static std::string render(const std::vector<Found> &found)
{
    if (found.empty())
        return "nothing";
    std::string s;
    for (auto const &f : found)  {
        if (!s.empty())
            s += ", ";
        s += f.object;
        s += f.instrumentable ? " (instrumentable)" : " (uninstrumentable)";
    }
    return s;
}


// A stub forwards rather than defines, which is why it is uninstrumentable and
// so why the flag governs it at all.  Assert that of whatever came out of the
// executable, so the test fails rather than quietly passing if a lookup ever
// starts reporting the definition from there.
static bool stubsAreUninstrumentable(const std::vector<Found> &found)
{
    for (auto const &f : found)  {
        if (f.object.find("mutatee") != std::string::npos && f.instrumentable)
            return false;
    }
    return true;
}


// The three by-name entry points, each asked the same question.
static std::vector<Found> lookupInImage(BPatch_image *img, const char *name,
                                        bool incUninstrumentable)
{
    std::vector<BPatch_function *> funcs;
    img->findFunction(name, funcs, false, true, incUninstrumentable);
    return describeAll(funcs);
}


static std::vector<Found> lookupInObject(BPatch_object *obj, const char *name,
                                         bool incUninstrumentable)
{
    std::vector<BPatch_function *> funcs;
    obj->findFunction(name, funcs, false, true, incUninstrumentable, true);
    return describeAll(funcs);
}


// A module lookup is confined to one module, and which module of the
// executable holds the stub is not something a test should assert, so ask
// every module the object has and take the union.
static std::vector<Found> lookupInModules(BPatch_object *obj, const char *name,
                                          bool incUninstrumentable)
{
    std::vector<BPatch_module *> mods;
    obj->modules(mods);

    std::vector<BPatch_function *> funcs;
    for (auto *mod : mods)
        mod->findFunction(name, funcs, false, true, incUninstrumentable, true);
    return describeAll(funcs);
}


static BPatch_object *objectNamed(BPatch_image *img, const std::string &what)
{
    std::vector<BPatch_object *> objects;
    img->getObjects(objects);
    for (auto *obj : objects)  {
        if (baseName(obj->pathName()).find(what) != std::string::npos)
            return obj;
    }
    return NULL;
}


// Every lookup of the imported name, for one scenario.
static void checkImportedName(BPatch_image *img, BPatch_object *exe, bool deps)
{
    const char *name = "plt_target";

    struct Probe
    {
        const char *label;
        std::vector<Found> (*fn)(BPatch_object *, const char *, bool);
    };

    // The whole address space, with and without the flag.
    auto imageOn = lookupInImage(img, name, true);
    auto imageOff = lookupInImage(img, name, false);
    std::printf("  image   incUninstrumentable=1 -> %s\n", render(imageOn).c_str());
    std::printf("  image   incUninstrumentable=0 -> %s\n", render(imageOff).c_str());

    check(countFrom(imageOn, "mutatee") == 1,
          "image, flag set: reports the stub");
    check(stubsAreUninstrumentable(imageOn),
          "image, flag set: the stub is uninstrumentable");
    check(countFrom(imageOff, "mutatee") == 0,
          "image, flag clear: reports no stub");
    if (deps)  {
        check(countFrom(imageOn, "libdef") == 1,
              "image, flag set: also reports the definition");
        check(imageOff.size() == 1 && countFrom(imageOff, "libdef") == 1,
              "image, flag clear: reports the definition and nothing else");
    }  else  {
        check(imageOn.size() == 1,
              "image, flag set: the stub is all there is");
        check(imageOff.empty(),
              "image, flag clear: reports nothing");
    }

    // The executable alone, by module and by object: it imports the name, so
    // only the stub can ever be found there.
    const Probe probes[] = { { "modules", lookupInModules },
                             { "object ", lookupInObject } };
    for (auto const &probe : probes)  {
        auto on = probe.fn(exe, name, true);
        auto off = probe.fn(exe, name, false);
        std::printf("  %s incUninstrumentable=1 -> %s\n", probe.label,
                    render(on).c_str());
        std::printf("  %s incUninstrumentable=0 -> %s\n", probe.label,
                    render(off).c_str());

        check(on.size() == 1 && countFrom(on, "mutatee") == 1,
              std::string(probe.label) + ", flag set: reports the stub, and only it");
        check(stubsAreUninstrumentable(on),
              std::string(probe.label) + ", flag set: the stub is uninstrumentable");
        check(off.empty(),
              std::string(probe.label) + ", flag clear: reports nothing");
    }
}


// The control: a name the executable defines.  No stub exists for it, so the
// flag makes no difference and a lookup never comes back empty.  Without this,
// a lookup that had stopped working entirely would satisfy every "reports no
// stub" check above.
static void checkDefinedName(BPatch_image *img, BPatch_object *exe)
{
    const char *name = "local_only";

    for (bool flag : { false, true })  {
        auto fromImage = lookupInImage(img, name, flag);
        auto fromObject = lookupInObject(exe, name, flag);
        std::printf("  control incUninstrumentable=%d -> image %s / object %s\n",
                    flag ? 1 : 0, render(fromImage).c_str(),
                    render(fromObject).c_str());

        std::string when = flag ? "flag set" : "flag clear";
        check(fromImage.size() == 1 && countFrom(fromImage, "mutatee") == 1
                  && fromImage[0].instrumentable,
              "control, image, " + when + ": the definition in the executable");
        check(fromObject.size() == 1 && fromObject[0].instrumentable,
              "control, object, " + when + ": the definition in the executable");
    }
}


static int runScenario(Scenario scenario)
{
    bool deps = scenario == Scenario::Deps;
    std::printf("-- %s\n", scenarioName(scenario).c_str());

    int before = failures;

    BPatch bpatch;
    std::string mutatee = std::string(ARTIFACT_DIR) + "/mutatee";
    BPatch_binaryEdit *be = bpatch.openBinary(mutatee.c_str(), deps);
    if (be == NULL)  {
        std::fprintf(stderr, "openBinary(%s, deps=%d) failed\n", mutatee.c_str(),
                     deps ? 1 : 0);
        return 2;
    }

    BPatch_image *img = be->getImage();
    BPatch_object *exe = objectNamed(img, "mutatee");
    if (exe == NULL)  {
        std::fprintf(stderr, "no object named mutatee in the image\n");
        return 2;
    }
    if (deps && objectNamed(img, "libdef") == NULL)  {
        std::fprintf(stderr, "libdef.so is not in the image; "
                             "the dependency was not opened\n");
        return 2;
    }

    checkImportedName(img, exe, deps);
    checkDefinedName(img, exe);

    return failures == before ? 0 : 1;
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

    int bad = 0;
    for (auto const &scenario : allScenarios())  {
        if (runScenario(scenario) != 0)
            bad = 1;
    }
    std::printf(bad ? "FAILURES\n" : "ALL PASS\n");
    return bad;
}
