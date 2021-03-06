//===-- driver.cpp - Clang GCC-Compatible Driver --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the entry point to the clang driver; it is a thin wrapper
// for functionality in the Driver clang library.
//
//===----------------------------------------------------------------------===//

#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Option.h"
#include "clang/Driver/Options.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Config/config.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/System/Host.h"
#include "llvm/System/Path.h"
#include "llvm/System/Signals.h"
using namespace clang;
using namespace clang::driver;

class DriverDiagnosticPrinter : public DiagnosticClient {
  std::string ProgName;
  llvm::raw_ostream &OS;

public:
  DriverDiagnosticPrinter(const std::string _ProgName, 
                          llvm::raw_ostream &_OS)
    : ProgName(_ProgName),
      OS(_OS) {}

  virtual void HandleDiagnostic(Diagnostic::Level DiagLevel,
                                const DiagnosticInfo &Info);
};

void DriverDiagnosticPrinter::HandleDiagnostic(Diagnostic::Level Level,
                                               const DiagnosticInfo &Info) {
  OS << ProgName << ": ";

  switch (Level) {
  case Diagnostic::Ignored: assert(0 && "Invalid diagnostic type");
  case Diagnostic::Note:    OS << "note: "; break;
  case Diagnostic::Warning: OS << "warning: "; break;
  case Diagnostic::Error:   OS << "error: "; break;
  case Diagnostic::Fatal:   OS << "fatal error: "; break;
  }
  
  llvm::SmallString<100> OutStr;
  Info.FormatDiagnostic(OutStr);
  OS.write(OutStr.begin(), OutStr.size());
  OS << '\n';
}

llvm::sys::Path GetExecutablePath(const char *Argv0) {
  // This just needs to be some symbol in the binary; C++ doesn't
  // allow taking the address of ::main however.
  void *P = (void*) (intptr_t) GetExecutablePath;
  return llvm::sys::Path::GetMainExecutable(Argv0, P);
}

static const char *SaveStringInSet(std::set<std::string> &SavedStrings, 
                                   const std::string &S) {
  return SavedStrings.insert(S).first->c_str();
}

/// ApplyQAOverride - Apply a list of edits to the input argument lists.
///
/// The input string is a space separate list of edits to perform,
/// they are applied in order to the input argument lists. Edits
/// should be one of the following forms:
///
///  '^': Add FOO as a new argument at the beginning of the command line.
///
///  '+': Add FOO as a new argument at the end of the command line.
///
///  's/XXX/YYY/': Replace the literal argument XXX by YYY in the
///  command line.
///
///  'xOPTION': Removes all instances of the literal argument OPTION.
///
///  'XOPTION': Removes all instances of the literal argument OPTION,
///  and the following argument.
///
///  'Ox': Removes all flags matching 'O' or 'O[sz0-9]' and adds 'Ox'
///  at the end of the command line.
void ApplyOneQAOverride(std::vector<const char*> &Args, 
                        const std::string &Edit,
                        std::set<std::string> &SavedStrings) {
  // This does not need to be efficient.

   if (Edit[0] == '^') {
     const char *Str = 
       SaveStringInSet(SavedStrings, Edit.substr(1, std::string::npos));
     llvm::errs() << "### Adding argument " << Str << " at beginning\n";
     Args.insert(Args.begin() + 1, Str);
   } else if (Edit[0] == '+') {
     const char *Str = 
       SaveStringInSet(SavedStrings, Edit.substr(1, std::string::npos));
     llvm::errs() << "### Adding argument " << Str << " at end\n";
     Args.push_back(Str);
   } else if (Edit[0] == 'x' || Edit[0] == 'X') {
     std::string Option = Edit.substr(1, std::string::npos);
     for (unsigned i = 1; i < Args.size();) {
       if (Option == Args[i]) {
         llvm::errs() << "### Deleting argument " << Args[i] << '\n';
         Args.erase(Args.begin() + i);
         if (Edit[0] == 'X') {
           if (i < Args.size()) {
             llvm::errs() << "### Deleting argument " << Args[i] << '\n';
             Args.erase(Args.begin() + i);
           } else
             llvm::errs() << "### Invalid X edit, end of command line!\n";
         }
       } else
         ++i;
     }
   } else if (Edit[0] == 'O') {
     for (unsigned i = 1; i < Args.size();) {
       const char *A = Args[i];
       if (A[0] == '-' && A[1] == 'O' && 
           (A[2] == '\0' || 
            (A[3] == '\0' && (A[2] == 's' || A[2] == 'z' ||
                              ('0' <= A[2] && A[2] <= '9'))))) {
         llvm::errs() << "### Deleting argument " << Args[i] << '\n';
         Args.erase(Args.begin() + i);
       } else
         ++i;
     }     
     llvm::errs() << "### Adding argument " << Edit << " at end\n";
     Args.push_back(SaveStringInSet(SavedStrings, '-' + Edit));
   } else {
     llvm::errs() << "### Unrecognized edit: " << Edit << "\n";
   }
}

/// ApplyQAOverride - Apply a comma separate list of edits to the
/// input argument lists. See ApplyOneQAOverride.
void ApplyQAOverride(std::vector<const char*> &Args, const char *OverrideStr,
                     std::set<std::string> &SavedStrings) {
  llvm::errs() << "### QA_OVERRIDE_GCC3_OPTIONS: " << OverrideStr << "\n";

  // This does not need to be efficient.

  const char *S = OverrideStr;
  while (*S) {
    const char *End = ::strchr(S, ' ');
    if (!End)
      End = S + strlen(S);
    if (End != S)
      ApplyOneQAOverride(Args, std::string(S, End), SavedStrings);
    S = End;
    if (*S != '\0')
      ++S;
  }
}

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal();
  llvm::PrettyStackTraceProgram X(argc, argv);

  llvm::sys::Path Path = GetExecutablePath(argv[0]);
  llvm::OwningPtr<DiagnosticClient> 
    DiagClient(new DriverDiagnosticPrinter(Path.getBasename(), llvm::errs()));

  Diagnostic Diags(DiagClient.get());

  llvm::OwningPtr<Driver> 
    TheDriver(new Driver(Path.getBasename().c_str(), Path.getDirname().c_str(),
                         llvm::sys::getHostTriple().c_str(),
                         "a.out", Diags));

  llvm::OwningPtr<Compilation> C;

  // Handle QA_OVERRIDE_GCC3_OPTIONS and CCC_ADD_ARGS, used for editing a
  // command line behind the scenes.
  std::set<std::string> SavedStrings;
  if (const char *OverrideStr = ::getenv("QA_OVERRIDE_GCC3_OPTIONS")) {
    // FIXME: Driver shouldn't take extra initial argument.
    std::vector<const char*> StringPointers(argv, argv + argc);

    ApplyQAOverride(StringPointers, OverrideStr, SavedStrings);

    C.reset(TheDriver->BuildCompilation(StringPointers.size(), 
                                        &StringPointers[0]));
  } else if (const char *Cur = ::getenv("CCC_ADD_ARGS")) {
    std::vector<const char*> StringPointers;

    // FIXME: Driver shouldn't take extra initial argument.
    StringPointers.push_back(argv[0]);

    for (;;) {
      const char *Next = strchr(Cur, ',');
      
      if (Next) {
        StringPointers.push_back(SaveStringInSet(SavedStrings,
                                                 std::string(Cur, Next)));
        Cur = Next + 1;
      } else {
        if (*Cur != '\0')
          StringPointers.push_back(SaveStringInSet(SavedStrings, Cur));
        break;
      }
    }

    StringPointers.insert(StringPointers.end(), argv + 1, argv + argc);

    C.reset(TheDriver->BuildCompilation(StringPointers.size(), 
                                        &StringPointers[0]));
  } else
    C.reset(TheDriver->BuildCompilation(argc, argv));

  int Res = 0;
  if (C.get())
    Res = C->Execute();

  llvm::llvm_shutdown();

  return Res;
}

