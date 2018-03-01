//
// Helper class used by various writers for additional configuration when
// reporting model status.
//

#include "helpers/AnyOption.h"

#include "SimInterface.h"
#include "ReportExtras.h"

namespace adl {

  static bool dummy ATTRIBUTE_UNUSED = 
    options().setFlag  ("show-all-cache-inits",           "Dump all initial cache lines to an output file, even if not valid. Default = false.\n") &&
    options().setFlag  ("show-all-cache-results",         "Dump all result cache lines to an output file, even if not valid. Default = false.\n") &&
    options().setFlag  ("show-all-mmu-inits",             "Dump all initial MMU translations to an output file, even if not valid. Default = false.\n") &&
    options().setFlag  ("show-all-mmu-results",           "Dump all result MMU translations to an output file, even if not valid. Default = false.\n");

  ReportExtras::ReportExtras() :
    _is_init(true),
    _show_all_cache_inits(options().getFlag("show-all-cache-inits",false)),
    _show_all_cache_results(options().getFlag("show-all-cache-results",false)),
    _show_all_mmu_inits(options().getFlag("show-all-mmu-inits",false)),
    _show_all_mmu_results(options().getFlag("show-all-mmu-results",false))
  {}

  bool ReportExtras::show_anyway(ShowType type)
  {
    if (type == ShowMmu) {
      // MMU display overrides.
      return _is_init ? _show_all_mmu_inits : _show_all_mmu_results;
    } else {
      // Cache display overrides.
      return _is_init ? _show_all_cache_inits : _show_all_cache_results;
    }
  }

}
