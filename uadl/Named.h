#ifndef _NAMED_H
#define _NAMED_H

#include <string>


namespace uadl {

class Named {
public:
  Named(const std::string &name) : _name(name) {}

  const std::string &name() const { return _name; }

private:
  const std::string _name;
};

} // namespace uadl

#endif // _NAMED_H
