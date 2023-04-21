#include "BPatch.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include <cstdio>

/*
 * The goal of this mutator is to rewrite a statically-linked executable
 * with some arbitrary modification. To validate the modification, the
 * simplest change is to insert a known call to `puts`. Here, we insert
 * `puts("Hello")` into the 'main' function.
 *
 * The output of the rewritten binary should be
 *
 *   Foo
 *   Hello
 *   ~Foo
 */

int main() {
  BPatch bpatch;
  BPatch_binaryEdit *edit = bpatch.openBinary("main");
  BPatch_image *image = edit->getImage();

  BPatch_Vector<BPatch_function *> mainFuncs;
  image->findFunction("main", mainFuncs);
  BPatch_function *main = mainFuncs[0];

  auto *mainEntryPoints = main->findPoint(BPatch_entry);
  BPatch_point *mainEntry = (*mainEntryPoints)[0];

  std::vector<BPatch_function *> putsFuncs;
  image->findFunction("puts", putsFuncs);
  if (putsFuncs.size() == 0) {
    fprintf(stderr, "Could not find 'puts'\n");
    return -1;
  }

  auto hello = BPatch_constExpr("Hello");
  std::vector<BPatch_snippet *> args = {&hello};
  BPatch_funcCallExpr putsCall(*(putsFuncs[0]), args);

  if (!edit->insertSnippet(putsCall, *mainEntry)) {
    fprintf(stderr, "insertSnippet failed\n");
    return -1;
  }

  edit->writeFile("main_rewritten");
}
