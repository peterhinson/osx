## Process this file with automake to produce Makefile.in

SUBDIRS=common functions math saxon sets strings dynamic date

test tests:
	@(cur=`pwd` ; for dir in $(SUBDIRS) ; do cd $$dir ; $(MAKE) CHECKER='$(CHECKER)' tests ; cd $$cur ; done)

valgrind:
	@echo '## Running the regression tests under Valgrind'
	$(MAKE) CHECKER='valgrind -q' tests

