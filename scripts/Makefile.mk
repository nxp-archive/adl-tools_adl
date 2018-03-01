
if !MINIMAL_BUILD

bin_SCRIPTS += scripts/adl2iss scripts/adl2asm scripts/adl2doc							\
							 scripts/uadl2model scripts/adl-config scripts/check_results	\
							 scripts/translate_directives scripts/usagelog.pm							\
							 scripts/xhtml2pdf scripts/makedat scripts/gdbclient

endif

dist_scripts = 	scripts/rdriver.pm \
	scripts/regress_utils.pm \
	scripts/adl_config.pm \
	scripts/adl_wrapper_support.pm \
	scripts/iss-macros.gdb \
	scripts/adl-dat.el

data_DATA += $(dist_scripts)

perlDBdatadir = $(datadir)/ADLPerlDB

perldb_files = 	\
	scripts/ADLPerlDB/InstrHandler.pm \
	scripts/ADLPerlDB/EncHelper.pm \
	scripts/ADLPerlDB/ExpandHelper.pm \
	scripts/ADLPerlDB/ADLPerlDB.pm \
	scripts/ADLPerlDB/BasicHandler.pm

perlDBdata_DATA = $(perldb_files)

EXTRA_DIST += \
	$(dist_scripts) \
	scripts/win-env-setup \
	scripts/win-test-setup \
	scripts/update-win-test \
	scripts/run-test \
	scripts/check_results \
	scripts/gdbclient \
	scripts/translate_directives \
	scripts/makedat \
	scripts/Data/Compare.pm \
	scripts/Data/Compare/Plugins.pod \
	scripts/Data/Compare/Plugins/Scalar/Numbers.pm \
	scripts/Data/Compare/Plugins/Scalar/Properties.pm \
	scripts/XML/Mini.pm \
	scripts/XML/Mini/Document.pm \
	scripts/XML/Mini/Node.pm \
	scripts/XML/Mini/Element.pm \
	scripts/XML/Mini/TreeComponent.pm \
	scripts/XML/Mini/Element/CData.pm \
	scripts/XML/Mini/Element/Entity.pm \
	scripts/XML/Mini/Element/Comment.pm \
	scripts/XML/Mini/Element/Header.pm \
	scripts/XML/Mini/Element/DocType.pm \
	scripts/Text/Glob.pm \
	scripts/File/Find/Rule.pm \
	scripts/Number/Compare.pm \
	$(perldb_files)
