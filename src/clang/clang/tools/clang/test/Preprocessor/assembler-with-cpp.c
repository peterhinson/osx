// RUN: clang-cc -x assembler-with-cpp -fdollars-in-identifiers=0 -E %s > %t &&

#ifndef __ASSEMBLER__
#error "__ASSEMBLER__ not defined"
#endif


// Invalid token pasting is ok. 
// RUN: grep '1: X .' %t &&
#define A X ## .
1: A

// Line markers are not linemarkers in .S files, they are passed through.
// RUN: grep '# 321' %t &&
# 321

// Unknown directives are passed through.
// RUN: grep '# B C' %t &&
# B C

// Unknown directives are expanded.
// RUN: grep '# BAR42' %t &&
#define D(x) BAR ## x
# D(42)

// Unmatched quotes are permitted.
// RUN: grep "2: '" %t &&
// RUN: grep '3: "' %t &&
2: '
3: "

// Empty char literals are ok.
// RUN: grep "4: ''" %t &&
4: ''


// Portions of invalid pasting should still expand as macros.
// rdar://6709206
// RUN: grep "5: expanded (" %t &&
#define M4 expanded
#define	M5() M4 ## (

5: M5()

// rdar://6804322
// RUN: grep -F "6: blarg $foo" %t &&
#define FOO(name)  name ## $foo
6: FOO(blarg)

// RUN: clang-cc -x assembler-with-cpp -fdollars-in-identifiers=1 -E %s > %t &&
// RUN: grep -F "7: blarg$foo" %t &&
#define FOO(name)  name ## $foo
7: FOO(blarg)


// RUN: true
