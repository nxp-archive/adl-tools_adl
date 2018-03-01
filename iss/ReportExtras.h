//
// Helper class used by various writers for additional configuration when
// reporting model status.
//

#ifndef _REPORT_EXTRAS_H_
#define _REPORT_EXTRAS_H_

namespace adl {

  class ReportExtras {
  public:
    ReportExtras();

  protected:
    enum ShowType { ShowMmu, ShowCache };

    bool show_anyway(ShowType type);

    bool             _is_init;       // True => We're in the init phase.

    bool             _show_all_cache_inits, _show_all_cache_results, _show_all_mmu_inits, _show_all_mmu_results;
  };

}

#endif
