//
// Simple driver for the UVP parser.  This just parses a UVP file and prints the commands found.
//

#include <string>
#include <iostream>
#include <exception>

#include "UvpParser.h"
#include "UvpParseTypes.h"

using namespace std;
using namespace adl;

const unsigned MaxCommand = 2;

struct MyUvpParser : public UvpParser {
  MyUvpParser(const std::string &fn,bool trace_parsing = false,bool trace_scanning = false) :
    UvpParser(fn,trace_parsing,trace_scanning),
    _cmd_count(0)
  {}

  void handle_block_begin(const YYLTYPE &loc,unique_ptr<UvpToken> type,unique_ptr<UvpToken> name) {
    cout << printLocation(loc) << ": Block " << (const UvpValue&)(*type.get()) << " : " << (const UvpValue&)(*name.get()) << " {\n\n";
  }

  void handle_block_end(const YYLTYPE &loc) {
    cout << printLocation(loc) << ": }\n\n";
  }

  void handle_cmd(const YYLTYPE &loc,unique_ptr<UvpCmd> cmd) {
    cout << printLocation(loc) << ": " << *cmd.get() << "\n\n";    
  }

  void handle_parm(const YYLTYPE &loc,unique_ptr<UvpToken> name,unique_ptr<UvpValue> value) {
    cout << printLocation(loc) << ": " << (const UvpValue&)(*name.get()) << " = " << *value.get() << "\n\n";
  }

  unsigned _cmd_count;
};

int main(int argc,char *argv[])
{
  if (argc < 2) {
    cout << "Usage:  " << argv[0] << " <file>\n\n";
  }

  bool trace_parsing  = false;
  bool trace_scanning = false;

  string fn;

  for (++argv; argv[0]; ++argv) {
    if (*argv == std::string ("-p")) {
      trace_parsing = true;
    } else if (*argv == std::string ("-s")) {
      trace_scanning = true;
    } else {
      fn = *argv;
    }
  }

  try {
    MyUvpParser parser(fn,trace_parsing,trace_scanning);

    while (!parser.is_finished()) {
      parser.parse();
    };
  }
  catch (exception &err) {
    cerr << err.what() << "\n\n";
    return 1;
  }
  return 0;
}

