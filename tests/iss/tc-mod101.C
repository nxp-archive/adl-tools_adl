//
// An example extension library for as-mod101.  This extends expression parsing
// to handle a special scale(x) intrinsic, where the scaling factor is specified
// via a command-line argument.  We also add on a pseudo-op.
//

extern "C" {
#include "as.h"
}

#include <assert.h>
#include <vector>
#include <string>
#include <cstring>

#include "adl-asm-impl.h"
#include "adl-asm-info.h"

using namespace std;
using namespace adl;

enum options
{
  OPTION_SCALE = OPTION_ADL_BASE
};

// This adds --scale=N to the set of valid options.
struct option md_longopts[] =
{
  {"scale", required_argument, NULL, OPTION_SCALE},
  DEFAULT_ADL_LONG_OPTS
};

const size_t md_longopts_size = sizeof (md_longopts);

static unsigned scale_factor = 1;

int acb_parse_option (int c,const char *arg)
{
  switch (c) {
  case OPTION_SCALE:
    scale_factor = atoi(arg);
    if (scale_factor == 0) {
      AS_BAD (_("bad scale factor (must be numerical and non-zero): %s"), arg);
      return 0;
    }
    return 1;
    break;
  }
  return 0;
}


segT acb_parse_expr(expressionS *ex,const char *arg)
{
  // Look for the special 'scale' pseudo-op.  If present, then the argument
  // should be an integer.  Otherwise, parse as a standard expression.
  const char *op = "scale(";
  unsigned op_len = strlen(op);
  if (strncasecmp(arg,op,op_len) == 0) {
    const char *p = strchr(arg+op_len,')');
    if (p) {
      unsigned val = atoi(arg+op_len);
      ex->X_add_number = val * scale_factor;
      ex->X_op = O_constant;
      return 0;
    }
  }
  
  return expression(ex);
}

void acb_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
  fprintf (stream, "\
--scale=N		Specify a scaling factor.\n"
           );
}


// Add on 'half' and 'word' directives for constants.
static const pseudo_typeS my_pseudo_table[] =
{
  {"half", cons, 2},
  {"word", cons, 4},
  { NULL, NULL, 0 },
};

void acb_pop_insert (void)
{
  extern void pop_insert (const pseudo_typeS *);

  pop_insert (my_pseudo_table);
}
