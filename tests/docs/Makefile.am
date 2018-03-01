
AUTOMAKE_OPTIONS = foreign serial-tests

if VALIDATE_XML
VALIDATE_TESTS=regress2
else
VALIDATE_TESTS=
endif

TESTS = regress $(VALIDATE_TESTS)

CLEANFILES = *.html *.rst

EXTRA_DIST = \
	$(srcdir)/regress \
	$(srcdir)/regress2 \
	$(srcdir)/*.pl \
	$(srcdir)/mod1.adl \
	$(srcdir)/doc*.adl \
	$(srcdir)/doc*.cfg \
	$(srcdir)/doc*.regress \
	$(srcdir)/xml-parse*.regress \
	$(srcdir)/spr-query.regress \
	$(srcdir)/microinstr*.*

if !CREATE_EXTERNAL_RELEASE

EXTRA_DIST += \
	$(srcdir)/booke.adl \
	$(srcdir)/zen.adl \
	$(srcdir)/zen0.adl \
	$(srcdir)/zen0.rst.regress \
	$(srcdir)/sc3400.pl \
	$(srcdir)/sc*.regress

endif

ADL2DOC = $(top_builddir)/scripts/adl2doc --prefix=$(top_builddir)

# Run check with arguments to run the regressions in parallel.
pcheck:
	$(MAKE) check RegressOpts="-p=8"

.rst.html:
	$(PREST) $< -W stylesheet=$(AdlStyle) > $@

.adl.html:
	$(ADL2DOC)  $<
