//--------------------------------------------------------------------*- C++ -*-
// CLING - the C++ LLVM-based InterpreterG :)
// author:  Vassil Vassilev <vasil.georgiev.vasilev@cern.ch>
//
// This file is dual-licensed: you can choose to license it under the University
// of Illinois Open Source License or the GNU Lesser General Public License. See
// LICENSE.TXT for details.
//------------------------------------------------------------------------------

#ifndef CLING_DECL_COLLECTOR_H
#define CLING_DECL_COLLECTOR_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTMutationListener.h"
#include "clang/Lex/PPCallbacks.h"

namespace clang {
  class ASTContext;
  class CodeGenerator;
  class Decl;
  class DeclGroupRef;
  class Token;
}

namespace cling {

  class Interpreter;
  class Transaction;

  ///\brief Collects declarations and fills them in cling::Transaction.
  ///
  /// cling::Transaction becomes is a main building block in the interpreter.
  /// cling::DeclCollector is responsible for appending all the declarations
  /// seen by clang.
  ///
  class DeclCollector: public clang::PPCallbacks,
                       public clang::ASTMutationListener,
                       public clang::ASTConsumer  {
  private:
    Transaction* m_CurTransaction;

    ///\brief Test whether the first decl of the DeclGroupRef comes from an AST
    /// file.
    ///
    bool comesFromASTReader(clang::DeclGroupRef DGR) const;
    bool comesFromASTReader(const clang::Decl* D) const;

  public:
    DeclCollector() : m_CurTransaction(0){}
    virtual ~DeclCollector();

    /// \name PPCallbacks overrides
    /// Macro support
    virtual void MacroDefined(const clang::Token &MacroNameTok,
                              const clang::MacroDirective *MD);
    /// \}

    /// \name ASTMutationListeners overrides
    virtual void AddedCXXImplicitMember(const clang::CXXRecordDecl *RD,
                                        const clang::Decl *D);
    /// \}

    /// \{
    /// \name ASTConsumer overrides

    virtual bool HandleTopLevelDecl(clang::DeclGroupRef DGR);
    virtual void HandleInterestingDecl(clang::DeclGroupRef DGR);
    virtual void HandleTagDeclDefinition(clang::TagDecl* TD);
    virtual void HandleInvalidTagDeclDefinition(clang::TagDecl* TD);
    virtual void HandleVTable(clang::CXXRecordDecl* RD,
                              bool DefinitionRequired);
    virtual void CompleteTentativeDefinition(clang::VarDecl* VD);
    virtual void HandleTranslationUnit(clang::ASTContext& Ctx);
    virtual void HandleCXXImplicitFunctionInstantiation(clang::FunctionDecl *D);
    virtual void HandleCXXStaticMemberVarInstantiation(clang::VarDecl *D);
    /// \}

    /// \{
    /// \name Transaction Support

    Transaction* getTransaction() { return m_CurTransaction; }
    const Transaction* getTransaction() const { return m_CurTransaction; }
    void setTransaction(Transaction* curT) { m_CurTransaction = curT; }
    void setTransaction(const Transaction* curT) {
      m_CurTransaction = const_cast<Transaction*>(curT);
    }

    /// \}

    // dyn_cast/isa support
    static bool classof(const clang::ASTConsumer*) { return true; }
  };
} // namespace cling

#endif // CLING_DECL_COLLECTOR_H
