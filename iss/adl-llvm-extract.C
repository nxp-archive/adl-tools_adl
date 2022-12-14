//===- llvm-extract.cpp - LLVM function extraction utility ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This utility changes the input module to only contain a single function,
// which is primarily used for debugging transformations.
//
// This is just replicated in the ADL environment so that I can modify the error
// handling behavior.  I need it to not produce an error if listed symbols don't
// exist.
//===----------------------------------------------------------------------===//

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Regex.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SetVector.h"
#include <memory>
using namespace llvm;

// InputFilename - The filename to read from.
static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
              cl::init("-"), cl::value_desc("filename"));

static cl::opt<std::string>
OutputFilename("o", cl::desc("Specify output filename"),
               cl::value_desc("filename"), cl::init("-"));

static cl::opt<bool>
Force("f", cl::desc("Enable binary output on terminals"));

static cl::opt<bool>
DeleteFn("delete", cl::desc("Delete specified Globals from Module"));

// ExtractFuncs - The functions to extract from the module.
static cl::list<std::string>
ExtractFuncs("func", cl::desc("Specify function to extract"),
             cl::ZeroOrMore, cl::value_desc("function"));

// ExtractRegExpFuncs - The functions, matched via regular expression, to 
// extract from the module.
static cl::list<std::string>
ExtractRegExpFuncs("rfunc", cl::desc("Specify function(s) to extract using a "
                                     "regular expression"),
                   cl::ZeroOrMore, cl::value_desc("rfunction"));

// ExtractGlobals - The globals to extract from the module.
static cl::list<std::string>
ExtractGlobals("glob", cl::desc("Specify global to extract"),
               cl::ZeroOrMore, cl::value_desc("global"));

// ExtractRegExpGlobals - The globals, matched via regular expression, to
// extract from the module...
static cl::list<std::string>
ExtractRegExpGlobals("rglob", cl::desc("Specify global(s) to extract using a "
                                       "regular expression"),
                     cl::ZeroOrMore, cl::value_desc("rglobal"));

static cl::opt<bool>
OutputAssembly("S",
               cl::desc("Write output as LLVM assembly"), cl::Hidden);

int main(int argc, char **argv) {
  // Print a stack trace if we signal out.
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);

  LLVMContext &Context = getGlobalContext();
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  cl::ParseCommandLineOptions(argc, argv, "llvm extractor\n");

  // Use lazy loading, since we only care about selected global values.
  SMDiagnostic Err;
  std::auto_ptr<Module> M;
  M.reset(getLazyIRFileModule(InputFilename, Err, Context));

  if (M.get() == 0) {
    Err.print(argv[0], errs());
    return 1;
  }

  // Use SetVector to avoid duplicates.
  SetVector<GlobalValue *> GVs;

  // Figure out which globals we should extract.
  for (size_t i = 0, e = ExtractGlobals.size(); i != e; ++i) {
    GlobalValue *GV = M.get()->getNamedGlobal(ExtractGlobals[i]);
    if (GV) {
      GVs.insert(GV);
    }
  }

  // Extract globals via regular expression matching.
  for (size_t i = 0, e = ExtractRegExpGlobals.size(); i != e; ++i) {
    std::string Error;
    Regex RegEx(ExtractRegExpGlobals[i]);
    if (!RegEx.isValid(Error)) {
      errs() << argv[0] << ": '" << ExtractRegExpGlobals[i] << "' "
        "invalid regex: " << Error;
    }
    bool match = false;
    for (Module::global_iterator GV = M.get()->global_begin(), 
           E = M.get()->global_end(); GV != E; GV++) {
      if (RegEx.match(GV->getName())) {
        GVs.insert(&*GV);
        match = true;
      }
    }
  }

  // Figure out which functions we should extract.
  for (size_t i = 0, e = ExtractFuncs.size(); i != e; ++i) {
    GlobalValue *GV = M.get()->getFunction(ExtractFuncs[i]);
    if (GV) {
      GVs.insert(GV);
    }
  }
  // Extract functions via regular expression matching.
  for (size_t i = 0, e = ExtractRegExpFuncs.size(); i != e; ++i) {
    std::string Error;
    StringRef RegExStr = ExtractRegExpFuncs[i];
    Regex RegEx(RegExStr);
    if (!RegEx.isValid(Error)) {
      errs() << argv[0] << ": '" << ExtractRegExpFuncs[i] << "' "
        "invalid regex: " << Error;
    }
    bool match = false;
    for (Module::iterator F = M.get()->begin(), E = M.get()->end(); F != E; 
         F++) {
      if (RegEx.match(F->getName())) {
        GVs.insert(&*F);
        match = true;
      }
    }
  }

  // Materialize requisite global values.
  if (!DeleteFn)
    for (size_t i = 0, e = GVs.size(); i != e; ++i) {
      GlobalValue *GV = GVs[i];
      if (GV->isMaterializable()) {
        std::string ErrInfo;
        if (GV->Materialize(&ErrInfo)) {
          errs() << argv[0] << ": error reading input: " << ErrInfo << "\n";
          return 1;
        }
      }
    }
  else {
    // Deleting. Materialize every GV that's *not* in GVs.
    SmallPtrSet<GlobalValue *, 8> GVSet(GVs.begin(), GVs.end());
    for (Module::global_iterator I = M->global_begin(), E = M->global_end();
         I != E; ++I) {
      GlobalVariable *G = I;
      if (!GVSet.count(G) && G->isMaterializable()) {
        std::string ErrInfo;
        if (G->Materialize(&ErrInfo)) {
          errs() << argv[0] << ": error reading input: " << ErrInfo << "\n";
          return 1;
        }
      }
    }
    for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I) {
      Function *F = I;
      if (!GVSet.count(F) && F->isMaterializable()) {
        std::string ErrInfo;
        if (F->Materialize(&ErrInfo)) {
          errs() << argv[0] << ": error reading input: " << ErrInfo << "\n";
          return 1;
        }
      }
    }
  }

  // In addition to deleting all other functions, we also want to spiff it
  // up a little bit.  Do this now.
  PassManager Passes;
  Passes.add(new TargetData(M.get())); // Use correct TargetData

  std::vector<GlobalValue*> Gvs(GVs.begin(), GVs.end());

  Passes.add(createGVExtractionPass(Gvs, DeleteFn));
  if (!DeleteFn)
    Passes.add(createGlobalDCEPass());           // Delete unreachable globals
  Passes.add(createStripDeadDebugInfoPass());    // Remove dead debug info
  Passes.add(createStripDeadPrototypesPass());   // Remove dead func decls

  std::string ErrorInfo;
  tool_output_file Out(OutputFilename.c_str(), ErrorInfo,
                       raw_fd_ostream::F_Binary);
  if (!ErrorInfo.empty()) {
    errs() << ErrorInfo << '\n';
    return 1;
  }

  if (OutputAssembly)
    Passes.add(createPrintModulePass(&Out.os()));
  else if (Force || !CheckBitcodeOutputToConsole(Out.os(), true))
    Passes.add(createBitcodeWriterPass(Out.os()));

  Passes.run(*M.get());

  // Declare success.
  Out.keep();

  return 0;
}
