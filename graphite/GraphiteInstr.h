//
// Instruction class for Graphite performance model integration.  This store
// information about a single instruction- its operands and an instruction type.
// This is created at translation time and then fed to the performance model
// when the instruction is executed.
//

#ifndef _GRAPHITE_INSTR_H
#define _GRAPHITE_INSTR_H

#include <iosfwd>
#include <vector>

namespace adl {

  class Operand {
  public:
    enum Type {
      REG,
      MEMORY,
      IMMEDIATE
    };

    enum Direction {
      READ,
      WRITE
    };

    Operand(Type type, unsigned value = 0, Direction direction = READ) : 
      _type(type), 
      _value(value), 
      _direction(direction)
    {}

    Operand() : _type(IMMEDIATE), _value(0), _direction(READ) {};

    Type           type() const { return _type; };
    unsigned      value() const { return _value; };
    Direction direction() const { return _direction; };

    Type      _type;
    unsigned  _value;
    Direction _direction;
  };

  std::ostream &operator<< (std::ostream &,const Operand &);

  typedef std::vector<Operand> OperandList;

  class Instruction {
  public:
    // The instruction types used will be model-dependent and generated in an
    // auxiliary header file.
    Instruction(unsigned type, OperandList &&operands) :
      _type(type), _operands(operands) {};

    Instruction() : _type(0) {};

    virtual ~Instruction() { };

    unsigned type() const { return _type; }
    void setTypeAndSize(unsigned t,unsigned size) { _type = t; _operands.resize(size); };
    const OperandList& getOperands() const { return _operands; }
    
    void setOperand(unsigned index,Operand::Type type,unsigned value = 0,Operand::Direction direction = Operand::READ)
    {
      _operands[index]._type      = type;
      _operands[index]._value     = value;
      _operands[index]._direction = direction;
    }

  private:
    friend std::ostream &operator<< (std::ostream &,const Instruction &);

    unsigned    _type;
    OperandList _operands;
  };

}

#endif
