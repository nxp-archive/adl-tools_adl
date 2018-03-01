//
// Plugin for installing a Graphite simple core model.
//

#include <stdexcept>
#include <sstream>
#include <memory>

#include "helpers/Macros.h"

#include "iss/ModelInterface.h"
#include "iss/Plugin.h"

#include "graphite/GraphiteCore.h"

#include "simple_core_model.h"

using namespace std;
using namespace adl;

const double Freq = 1; // in GHz

struct PerfCorePlugin : public adl::Plugin {

  // Recursively install performance models into cores.
  void installPerfModel(IssNode &n)
  {
    if (n.is_core()) {
      if (!n.hasGraphitePerfSupport()) {
        RError("Core " << n.name() << " lacks support for Graphite performance models.");
      }
      _perf_models.emplace_back(new SimpleCoreModel(dynamic_cast<IssCore&>(n),Freq));
      n.setGraphitePerfModel(_perf_models.back().get());
    } else {
      for (auto c : n.children()) {
        installPerfModel(*c);
      }
    }
  }

  // Install a Graphite performance model into each core in the system tree.
  virtual void setup(AnyOption &options,IssNode &root)
  {
    installPerfModel(root);
  }

  // This invokes the statistics reporters for each model.
  virtual void finalize(IssNode &root)
  {
    cout << "Graphite Performance Model Results:\n\n";

    for (auto &p : _perf_models) {
      p->outputSummary(cout,p->core().name());
    }

    cout << "\n\n";
  }

  vector<unique_ptr<CoreModel> > _perf_models;
};

static PerfCorePlugin perf_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &perf_plugin;
}
