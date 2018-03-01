
if !MINIMAL_BUILD

if LANG_FRONTEND

SUBDIRS = binutils-adl
BINUTIL = binutils-adl
BINSRC  = $(shell cd $(srcdir) && pwd)/$(BINUTIL)

BASESRCDIR=$(shell cd $(top_srcdir) && pwd)
BASEDIR=$(shell cd $(top_builddir) && pwd)

CLEANFILES = *.bz2

install_pfx = binutils

binutils_dir = "$(DESTDIR)$(datadir)/$(install_pfx)"

# We copy over files needed for building user-generated assemblers and
# disassemblers.  This is kind of a hack- we should clean this up so that we
# don't need to copy object files.
install_dirs = \
	gas \
	gas/config \
	include \
	include/elf \
	binutils \
	opcodes \
	bfd \
	intl \
	libiberty \
	ld

asm_disasm_install_files = \
	gas/libgas-dt.a \
	gas/libgas.a \
	gas/libgas-gas.a \
	opcodes/libopcodes.a \
	libiberty/libiberty.a \
	bfd/libbfd.a \
	bfd/.libs/libbfd.a \
	binutils/libobjdump.a \
	binutils/libaddr2line.a \
	ld/libld.a

# The install process installs files needed for the "build locally" option and
# also installs a copy of the binutils tarball.
install-recursive:
	$(MAKE) install-am
	$(MAKE) install-build-locally-files

# This should remove both the tarball and the files in the share directory
# copied over for the build-locally option.
uninstall-recursive:
	$(MAKE) uninstall-am
	rm -rf $(binutils_dir)

install-build-locally-files:
	list='$(install_dirs)'; for dir in $$list; do \
		install -d $(binutils_dir)/$$dir; \
		(cd binutils-adl && for file in $$dir/*.h; do \
			cp $$file $(binutils_dir)/$$file ; \
		done); \
		if [ $(top_srcdir) != $(top_builddir) ] ; then \
			(cd $(top_srcdir)/external/binutils-adl && for file in $$dir/*.h; do \
				cp $$file $(binutils_dir)/$$file ; \
			done); \
		fi; \
	done
	list='$(asm_disasm_install_files)'; for file in $$list; do \
		(cd binutils-adl && cp $$file $(binutils_dir)/$$file ); \
	done

check-recursive: all-recursive
	@echo "Binutils checks disabled for now."

dvi-recursive:
	@echo "Binutils DVI building disabled for now."

installcheck:
	@:

distuninstallcheck:
	@:

endif LANG_FRONTEND

else

distclean:

clean:

endif
