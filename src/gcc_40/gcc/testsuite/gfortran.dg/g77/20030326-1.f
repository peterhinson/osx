C { dg-do compile }
C      PR fortran/9793
C      larson@w6yx.stanford.edu
C
       integer a, b, c

       c = -2147483648 / -1 ! { dg-bogus "Arithmetic overflow" "Arithmetic overflow" { xfail *-*-* } } PR 16512

       a = 1
       b = 0
       c = a / b

       print *, c

       end
