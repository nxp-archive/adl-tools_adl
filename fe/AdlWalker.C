//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is the visitor which walks the initial input.
//

#include <iostream>
#include <sstream>

#include "opencxx/parser/Parser.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeDeclaration.h"
#include "opencxx/parser/PtreeDeclarator.h"
#include "opencxx/parser/PtreeTemplateDecl.h"
#include "opencxx/parser/PtreeExprStatement.h"
#include "opencxx/parser/TheErrorLog.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/ErrorLog.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/BindEnumName.h"
#include "opencxx/Environment.h"
#include "opencxx/PtreeIter.h"

#include "helpers/fe_exceptions.h"
#include "helpers/Macros.h"

#include "Helpers.h"
#include "AdlWalker.h"
#include "Handler.h"
#include "BaseParser.h"
#include "BaseMsg.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  AdlWalker::AdlWalker(Parser *p,BaseParser &bp,MsgLog &ml) : Walker(p), _fullpath(true), _ml(ml)
  {
    // Add the default handler to the stack.
    _outer = bp.make_initial_handler(env);
    _handlers.push_back(_outer);
  }

  static Ptree *makeAddExpr(Ptree *left,unsigned right_value)
  {
    return new PtreeInfixExpr(left, PtreeUtil::List(new Leaf("+",1), make_leaf(right_value)));
  }
  
  Ptree* AdlWalker::TranslateEnumSpec(Ptree* spec)
  {
    Ptree *result = Walker::TranslateEnumSpec(spec);
 
    // Now add each enum name to the environment (this should be done by the
    // Walker in Plasma, but it's not).
    Ptree *values = Second(Third(spec));

    char* encoded_name = (char*)spec->GetEncodedName();

    unsigned count = 0;
    Ptree *last_value = 0;
    while(values != 0){
      Ptree* p = values->Car();
      Ptree *ident = 0;
      Ptree *opt = 0;
      if (p->IsLeaf()) {
        // Just an enum identifier, so we have to assume a value.  If we haven't
        // encountered any explicit values yet, then just keep creatintg leaf
        // items which store an incrementing value.  Otherwiose, we construct an
        // infix addition expression adding on a value to the last known set
        // value.
        ident = p;
        opt = (last_value) ? makeAddExpr(last_value,count): make_leaf(count);
        ++count;
      } else {
        // Value set by user, so reset the count and store this item as the last
        // value, for the creation of future addition expressions.
        ident = First(p);
        last_value = opt = Third(p);
        count = 1;
      }
      env->AddEntry(ident->GetPosition(),ident->GetLength(),new BindEnumName(encoded_name,opt));
      values = values->Cdr();
      if(values != 0){
        values = values->Cdr();
      }
    }  
    return result;
  }

  // This is called when an extended piece of syntax is encountered.
  Ptree* AdlWalker::TranslateUserPlain(Ptree*exp)
  {
    Ptree *nexp = 0;
    try {
      // If we have any syntax errors, skip this stuff- just continue
      // checking syntax by parsing.
      if (!parser->SyntaxErrorsFound()) {
        TypeInfo type;
        Ptree* keyword = First(exp);
        Ptree* rest = Rest(exp);
        // If this is extended syntax that we recognize, we handle it here.
        // Otherwise, it's an error.
        NewScope();
        if (Eq(keyword,"func")) {
          // At this point, we pass func statements through w/o transforming them.
          nexp = exp;
        }
        else if (Eq(keyword,"define") || Eq(keyword,"defmod")) {
          translateDefine(keyword,rest);
          // We always return null for user-plain stuff so that we can distinguish
          // it from other C++ code.
          nexp = 0;
        } 
        else {
          error("Unknown user statement encountered:  ",keyword,keyword);
        }
        ExitScope();
      }
    }
    catch (parse_warning &wrn) {
      warning(wrn.what(),0,exp);
    }
    catch (parse_error &err) {
      error(err.what(),0,exp);
    }
    return nexp;
  }

  // Handle a single define with a string name.
  void AdlWalker::handleDefine(Ptree *loc,const char *type,const char *name,
                               bool is_modify,Ptree *body)
  {
    Handler *h = 0;
    // The current handler creates a new handler for this block.  We then
    // add it to the stack, process the block's body, then pop the stack.
    h = handler().add_define(_ml,env,loc,type,name,is_modify);
    assert(h);
    _handlers.push_back(h);
    walk_body(body);
    _handlers.pop_back();
  }

  // Handle a single define with a numerical name.
  void AdlWalker::handleDefine(Ptree *loc,const char *type,unsigned id,
                               bool is_modify,Ptree *body)
  {
    Handler *h = 0;
    // The current handler creates a new handler for this block.  We then
    // add it to the stack, process the block's body, then pop the stack.
    h = handler().add_define(_ml,env,loc,type,id,is_modify);
    assert(h);
    _handlers.push_back(h);
    walk_body(body);
    _handlers.pop_back();
  }

  // Handle a define for group. Walks though all elements in the given group.
  void AdlWalker::handleGroupDefine(Ptree *loc,const char *type,const char *name,Ptree *body)
  {
    Handlers hl;
    // The current handler creates a new handler for this block.  We then
    // add it to the stack, process the block's body, then pop the stack.
    Handler *h = handler().add_defines(_ml,env,loc,type,name);
    // Evaluation of the group will be delayed until later.
    assert(h);
    _handlers.push_back(h);
    walk_body(body);
    _handlers.pop_back();  
  }

  void AdlWalker::handleDefineBlock(Ptree *name,Ptree *type,const char *type_str,bool is_modify,Ptree *body)
  {
    const char *name_str = 0;
    unsigned id;

    if (get_uint(id,name,env)) {
      // Are we dealing with a numerical name?
      handleDefine(type,type_str,id,is_modify,body);            
    } else {
      // Otherwise, must have a string or group name.
      Ptree* l;
      if(Match(name,"[* %?]",&l)) {              
        name_str = get_str(l,"defmod group name");
        handleGroupDefine(type,type_str,name_str,body);
      } else {
        name_str = get_str(name,"define/defmod name");
        handleDefine(type,type_str,name_str,is_modify,body);
      }              
    }
  }

  // Handle define blocks: We create a new Handler, push it onto our stack,
  // then continue parsing.  When done, we pop it from the stack.
  void AdlWalker::translateDefine(Ptree* keyword, Ptree* rest)
  {
    Ptree *type,*bad,*extra,*body;
    if (Match(rest,"[ ([[%? [%?] %r ]]) [ %r ] ]",&type,&bad,&extra,&body)) {
      const char *type_str = get_simple_ident(type,"type");
      bool is_modify = Eq(keyword,"defmod");
      if (bad) {        
        if (extra) {
          error("Malformed block:  ",bad,bad);
        } else {
          // Alternate form: The user has used the "define (x y) {}" block form.
          // This only parses as a single item, so we just make sure that it's
          // an identifier (which it should be or else it wouldn't parse).
          handleDefineBlock(bad,type,type_str,is_modify,body);
        }
      } else {
        Ptree *n,*list;
        if (Match(extra,"[ = %? ]",&n)) {        
          // For the name, we can take either a list of items or a single item.
          if ( !(list = get_flat_list(n)) ) {
            // Not a flat list, so assume that this is a single item.
            list = PtreeUtil::List(n);
          }
          // Now iterate over the list.
          Opencxx::PtreeIter iter(list);
          Opencxx::Ptree *p;
          while ( (p = iter())) {         
            handleDefineBlock(p,type,type_str,is_modify,body);
          }
        } else {
          // Unnamed case:  Call the character-string version by default.
          handleDefine(type,type_str,(const char*)0,is_modify,body);
        }
      }
    } else {
      error("Malformed define block:  ",keyword,rest);
    }

  }

  // Figures out what kind of declaration we're dealing with and
  // adds it to the appropriate data structure.
  void AdlWalker::add_decl(Ptree *elem,Ptree *body,bool no_decls)
  {
    Ptree *key,*value;
    Ptree *decls = Third(body);
    if (decls->IsA(ntDeclarator)) {
      PtreeDeclarator *t = dynamic_cast<Opencxx::PtreeDeclarator*>(decls);
      handler().add_func(t->Name()->ToString(),Translate(elem));
    } else if (PtreeUtil::Match(body,"[ [] %? [%?] ; ]",&key,&value)) {
      Ptree *var,*cnt;
      unsigned count = 0;
      bool is_array = false;
      if (PtreeUtil::Match(value,"[ %? %[ %? %] ]",&var,&cnt)) {
        // Array declaration, so extract the count value.
        is_array = true;
        count = get_uint(cnt," array size.",env);
      } else {
        // Not an array declaration.
        var = First(value);
      }
      // Handle declarations, if allowed.
      if (no_decls) {
        if (is_array) {
          error("An array declaration is not allowed here.",0,elem);
        }
        handler().add_type_decl(Translate(elem));        
      } else {
        const char *decl_type = get_simple_ident(key,"the declaration type");
        if (strcmp(decl_type,"remove") == 0) {
          // This is a remove command.
          if (is_array) {
            error("An array declaration is not allowed here.",0,elem);
            return;
          }
          const char *name = get_simple_ident(Translate(var),"a key to be removed");
          handler().add_remove(name);
        } else {
          Ptree *var2 = Translate(var);
          if (is_array) {
            ForRange(count,i) {
              Ptree *vn = Ptree::Make("%p%d",var2,i);
              handler().add_decl(decl_type,vn);
            }
          } else {
            handler().add_decl(decl_type,var2);
          }
        }
      }
    } else if (PtreeUtil::Match(body,"[ [] %? ; ]",&value)) {
      // Handle a type declaration.
      handler().add_type_decl(Translate(elem));
    } else {
      //cerr << "Bad item type:  " << typeid(*p).name() << ", " << p->What() << "\n\n";
      //p->Display(cerr);
      error("Unable to handle this type of declaration here.",0,elem);
    }
  }
  
  // A define/defmod may have:
  // 1.  Key/value pairs.
  // 2.  Another define/defmod.
  // 4.  A bare string (for documentation).  One or more are allowed at
  //     the beginning of a define/defmod.
  void AdlWalker::walk_body(Ptree *body)
  {
    PtreeIter iter(Second(body));
    Ptree *p;

    bool string_okay = true;

    while ( (p = iter()) ) {
      Ptree *key,*value;
      try {
        if (dynamic_cast<Opencxx::PtreeExprStatement*>(p) && 
            PtreeUtil::Match(p,"[ [%? = %?] ; ]",&key,&value)) {
          // Handle key/value pairs.
          const char *key_str = get_str(key,"the key");
          handler().add_key(key_str,Translate(value));
          //handler().replace_key(key_str,Translate(value));
          //} else if (dynamic_cast<Opencxx::PtreeExprStatement*>(p) && 
          //    PtreeUtil::Match(p,"[ [%? += %?] ; ]",&key,&value)) {
          //  // Handle key/value pairs.
          //  const char *key_str = get_str(key,"the key");
          //  handler().add_key(key_str,Translate(value));
        } else if (dynamic_cast<Opencxx::PtreeDeclaration*>(p)) {
          add_decl(p,p,false);
        } else if (dynamic_cast<Opencxx::PtreeTemplateDecl*>(p) ) {
          Ptree* body = PtreeUtil::Nth(p, 4);
          add_decl(p,body,true);
        } else if (Eq(p->Car(),"define") || Eq(p->Car(),"defmod")) {
          // Nested define.
          Translate(p);
        } else {
          Ptree *doc = Ca_ar(p);
          if (doc && doc->IsLeaf() && (*(doc->GetPosition()) == '"')) {
            if (string_okay) {
              handler().add_doc_string(doc);
            } else {
              error("Documentation strings are only allowed at the beginning of a define/defmod block",
                    0,p);
            }
          } else if (PtreeUtil::Match(p,"[ [] ; ]")) {
            // Skip.
          } else {
            // You're not allowed anything else within a define or defmod.
            //cerr << "Bad item type:  " << typeid(*p).name() << "\n\n";
            error("Invalid item within a define/defmod block:  ",p,p);
          }
          string_okay = false;
        }
      }
      catch (parse_warning &wrn) {
        warning(wrn.what(),0,wrn.loc());
      }
      catch (parse_error &err) {
        error(err.what(),0,err.loc());
      }
    }
  }

  void message(Msg::Severity severity, Parser* parser, 
               const char* msg, Ptree* name, Ptree* where,
               bool fullpath)
  {
    assert(parser);
    assert(where);
    SourceLocation location(parser->GetLocation(where));
    ostringstream buf;
    buf << msg << name;
    TheErrorLog().Report(BaseMsg(severity, location, buf.str(), fullpath));
  }


  // This issues a fatal error, which causes an exception to be thrown.  It attempts to
  // find a leaf in 'where' by traversing the list, calling Ca_ar() on each item until
  // it finds a leaf.
  void AdlWalker::fatal(const char *msg,Opencxx::Ptree *item,Opencxx::Ptree *where)
  {
    message(Msg::Fatal,parser,msg,item,where,_fullpath);
  }

  void AdlWalker::error(const char *msg,Opencxx::Ptree *item,Opencxx::Ptree *where)
  {
    message(Msg::Error,parser,msg,item,where,_fullpath);
  }

  void AdlWalker::warning(const char *msg,Opencxx::Ptree *item,Opencxx::Ptree *where)
  {
    message(Msg::Warning,parser,msg,item,where,_fullpath);
  }

}

