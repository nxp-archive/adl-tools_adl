//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This contains a hash storing information about all valid transaction types.
//

#ifndef _TRANSACTION_TYPES_
#define _TRANSACTION_TYPES_

#include "helpers/gc_string.h"

namespace uadl {

  class Transaction;

  // transaction types
  enum TransactionType {
    UNKNOWN_TRAN = -1,
    PREDICATE = 0, 
    INQUIRE_TRAN, 
    READ_TRAN, 
    ALLOCATE_TRAN, 
    EXEC_TRAN,
    COMMIT_TRAN, 
    WRITE_TRAN, 
    REQUEST_TRAN, 
    RELEASE_TRAN, 
    FLUSH_TRAN,
    MAX_TRAN_TYPE, 
  };

  // This can detect whether something is a pure predicate and will return
  // PREDICATE in such a case.
  TransactionType findTransactionType(const Transaction &trans);

  TransactionType findTransactionType(const gc_string &name);

}

#endif
