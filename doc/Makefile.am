
AUTOMAKE_OPTIONS = foreign

web_DATA = \
	adl-language-reference.html \
	uadl-manual.html \
	adl-gdb-howto.html \
	adl-iss-integration.html \
	dat-format.html \
	adl-fe-design.html \
	adl-iss-design.html \
	adl-iss-cli.html \
	adl-iss-plugins.html \
	adl-requirements.html \
	adl-db.html \
	methodology-notes.html \
	data-model-diagram.png \
	handler-diagram.png \
	parsing-flow.png \
	adl-language.png \
	$(uadl_png_figs) \
	adldoc.css \
	default-rst.css \
	release-notes.css \
	developers_guide.html \
	perlDB.html

if !IS_EXTERNAL_RELEASE

if !CREATE_EXTERNAL_RELEASE

if FOPOK
ADL_OVERVIEW = internal-doc/ADL-Overview.pdf internal-doc/adl-dto-presentation.pdf
else
ADL_OVERVIEW =
endif

endif


web_DATA += \
	internal-doc/adl-dto-presentation.pdf

endif !IS_EXTERNAL_RELEASE

INFO = $(srcdir)/info.xml

PDFS = \
	adl-language-reference.pdf \
	uadl-manual.pdf \
	dat-format.pdf \
	adl-gdb-howto.pdf \
	adl-iss-cli.pdf \
	adl-iss-plugins.pdf \
	adl-iss-integration.pdf \
	adl-db.pdf

# Only build a pdf if FOP is good.
if FOPOK
web_DATA += $(PDFS)
endif

uadl_png_figs = OSM.png safe-mode-fetch-hit.png safe-mode-fetch-miss.png safe-mode-load-miss.png safe-mode-load-hit.png safe-mode-store-miss.png safe-mode-store-hit.png safe-mode-overlapped-accesses.png
uadl_pdf_figs = $(uadl_png_figs:%.png=%.pdf)

rst_src = $(wildcard $(srcdir)/*.rst)

infodir = $(InfoHome)
webdir  = $(WebDir)
reldir  = $(RelDir)

FslHeader = $(srcdir)/fsl-header.rst

BASEDIR=$(shell cd $(top_builddir) && pwd)

Xhtml2Pdf = $(top_builddir)/scripts/xhtml2pdf --prefix=$(BASEDIR)

EXTRA_DIST = \
	$(rst_src) \
	$(wildcard $(srcdir)/*.eps) \
	$(wildcard $(srcdir)/*.css) \
	$(wildcard $(srcdir)/*.html *.html) \
	$(wildcard *.pdf) \
	$(wildcard $(srcdir)/*.png) \
	$(wildcard $(srcdir)/*.odg) \
	$(wildcard $(srcdir)/*.fig) \
	$(ADL_OVERVIEW)

FIGS     = doc/uadl/fig/OSM doc/uadl/fig/OSM_trans_spawn_only doc/uadl/fig/OSM_trans_spawn_other

FIG_FIGS = $(FIGS:%=%.fig)
EPS_FIGS = $(FIGS:%=%.eps)
PDF_FIGS = $(FIGS:%=%.pdf)
BAK_FIGS = $(FIG_FIGS:%=%.bak)

CLEANFILES = $(EPS_FIGS) $(PDF_FIGS) $(BAK_FIGS)

EXTRA_DIST += $(srcdir)/uadl/fig/*.fig

DOCCLEANFILES = *.html *.tex *.toc *.aux *.log $(PDFS) *.pdf.html

DISTFILE = $(PACKAGE)-$(VERSION).tar.bz2

SIDEDISTFILE = $(PACKAGE)-$(VERSION).$(VERSFX).tar.bz2

MakeRelNotes = $(top_srcdir)/scripts/make-release-notes

ReadMe = $(top_srcdir)/readme-$(VERSION).html

PrestFlags = -W stylesheet="default-rst.css" -D keep_title_section

# Just call firefox unless the user has already set it in their environment.
Browser ?= firefox

fig2dev = fig2dev

install:
	@echo "***"
	@echo "*** Documentation is not installed by default."
	@echo "*** Use the 'docinstall' target to install it."
	@echo "***"

uninstall:
	@echo "Uninstall not implemented for documentation."

docclean:
	rm -f $(DOCCLEANFILES)

# This installs just the documentation.
web-data-install:
	$(MAKE) install-webDATA ;\
	if [ ! -z $(infodir) -a -e $(infodir) -a -f $(INFO) ] ; then \
		echo " $(install_sh_DATA) $(INFO) $(infodir)/$(INFO)"; \
		$(install_sh_DATA) $(INFO) $(infodir)/$(INFO); \
	fi

# Generate the release notes files.  
#
# Note: We're inserting the style-sheet file into the generated README because I
# don't know of of a good location to put a stylesheet.  I would say that it's
# Compass, but since they turned off public access, it's only visible if you log
# in.

show-release-notes: gen-release-notes
	@$(Browser) $(ReadMe)

gen-release-notes: trac-gen-release-notes

oss-gen-release-notes: oss-trac-gen-release-notes

MakeTracRelNotes = $(MakeRelNotes) \
		--topdir=$(top_srcdir) \
		--proj=ADL \
		--query-proj=adl \
		--ver=$(VERSION) \
		--output=$(ReadMe) \
		--style-file=$(top_srcdir)/doc/release-notes.css \
		--iface-check=iss/ModelInterface.h \
		--iface-check=uadl/UadlArch.h \
		--iface-check=uadl/UadlArchSystemC.h

# Trac version for internal release.  Will contain user info for how to install.
trac-gen-release-notes:
	$(MakeTracRelNotes) --prefix=$(prefix) --header-file=$(top_srcdir)/doc/release-notes-header.html $(RelNoteFlags)

# Trac version for open-source releases.  No prefix, since it's meant for the
# outside world.
oss-trac-gen-release-notes:
	$(MakeTracRelNotes) --header-file=$(top_srcdir)/doc/oss-release-notes-header.html $(RelNoteFlags)

# Creates the release notes and places them in the web directory.
release-notes: gen-release-notes
	$(mkinstalldirs) $(DESTDIR)$(reldir) ; \
	$(install_sh_DATA) $(ReadMe) $(DESTDIR)$(reldir) ;

web-dist:
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) dist-bzip2

side-web-dist:
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) dist-bzip2 distdir=$(PACKAGE)-$(VERSION).$(VERSFX)

# This installs the documentation and creates a distribution which is then
# copied to the distribution directory.
docinstall: all-am release-notes web-data-install web-dist
    ## Final step:  Make a distribution and copy it to the web directory.
	$(mkinstalldirs) $(DESTDIR)$(reldir) ; \
	$(install_sh_DATA) $(top_builddir)/$(DISTFILE) $(DESTDIR)$(reldir) ;\
	rm -f $(top_builddir)/$(DISTFILE)

side-dist: side-web-dist
	$(mkinstalldirs) $(DESTDIR)$(reldir) ; \
	$(install_sh_DATA) $(top_builddir)/$(SIDEDISTFILE) $(DESTDIR)$(reldir) ;\
	rm -f $(top_builddir)/$(SIDEDISTFILE)

adl-language-reference.pdf: adl-language.pdf

adl-iss-integration.pdf adl-notes-integration.html: adl-iss-integration.rst $(top_srcdir)/tests/scripts/driver2.C

uadl-manual.html: uadl-manual.rst $(uadl_png_figs)
uadl-manual.tex: uadl-manual.rst $(uadl_pdf_figs)

%.pdf.html : %.rst
	if [ -f $@ ] ; then chmod u+w $@ ; fi
	$(PREST) $(PrestFlags) -D source-link=0 -D generator=0 $< > $@

%.pdf : %.pdf.html ;
	$(Xhtml2Pdf) $< --output=$@

%.html : %.rst fsl-header.rst;
	if [ -f $@ ] ; then chmod u+w $@ ; fi
	$(PREST) $(PrestFlags) $(FslHeader) $< > $@

%.xhtml : %.rst fsl-header.rst;
	if [ -f $@ ] ; then chmod u+w $@ ; fi
	$(PREST) $(PrestFlags) $< > $@

#OSM.pdf: OSM.fig
#	$(fig2dev) -L pdf -p 1 -m 0.5 $< $@

%.pdf: %.fig
	$(fig2dev) -L pdf $< $@

%.png: %.fig
	$(fig2dev) -L png $< $@

%.png: %.eps
	$(CONVERT) $< $@

%.pdf: %.eps
	epstopdf --outfile=$@ $<
