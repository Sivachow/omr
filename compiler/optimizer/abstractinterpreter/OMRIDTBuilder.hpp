/*******************************************************************************
 * Copyright (c) 2020, 2020 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#ifndef OMR_IDT_BUILDER_INCL
#define OMR_IDT_BUILDER_INCL

/*
 * The following #define and typedef must appear before any #includes in this file
 */
#ifndef OMR_IDT_BUILDER_CONNECTOR
#define OMR_IDT_BUILDER_CONNECTOR
namespace OMR { class IDTBuilder; }
namespace OMR { typedef OMR::IDTBuilder IDTBuilderConnector; }
#endif

#include "il/ResolvedMethodSymbol.hpp"
#include "env/Region.hpp"
#include "compile/Compilation.hpp"
#include "optimizer/Inliner.hpp"
#include "optimizer/abstractinterpreter/AbsVisitor.hpp"
#include "optimizer/abstractinterpreter/IDT.hpp"
#include "optimizer/abstractinterpreter/IDTNode.hpp"

namespace TR { class IDTBuilder; }
namespace TR { class IDTBuilderVisitor; }

namespace OMR
{

class IDTBuilder
   {
   friend class ::TR::IDTBuilderVisitor;

   public:
   
   IDTBuilder(TR::ResolvedMethodSymbol* symbol, int32_t budget, TR::Region& region, TR::Compilation* comp, TR_InlinerBase* inliner);
   
   /**
    * @brief building the IDT in DFS order.
    * It starts from creating the root IDTNode using the _rootSymbol
    * and then builds the IDT recursively.
    * It stops when no more call site is found or the budget runs out.
    * 
    * @return the inlining dependency tree
    * 
    */
   TR::IDT* buildIDT();

   TR::IDTBuilder* self();
   
   protected:

   TR::Compilation* comp() { return _comp; };
   TR::Region& region() { return _region; };
   TR_InlinerBase* getInliner()  { return _inliner; };

   /**
    * @brief generate the control flow graph of a call target so that the abstract interpretation can use. 
    * 
    * @note: This method needs language specific implementation.
    *
    * @param callTarget the call target to generate CFG for
    * @return the control flow graph
    */
   TR::CFG* generateControlFlowGraph(TR_CallTarget* callTarget) { TR_UNIMPLEMENTED(); return NULL; }

   /**
    * @brief Perform the abstract interpretation on the method in the IDTNode. 
    * 
    * @note: This method needs language specific implementation.
    *
    * @param node the node to be abstract interpreted
    * @param visitor the visitor which defines the callback method 
    *                that will be called when visiting a call site during abtract interpretation.
    * @param arguments the arguments are the AbsValues passed from the caller method.
    * @param callerIndex the caller index
    */
   void performAbstractInterpretation(TR::IDTNode* node, TR::IDTBuilderVisitor& visitor, std::vector<TR::AbsValue*>* arguments, int32_t callerIndex) { TR_UNIMPLEMENTED(); }

   /**
    * @param node the node to build a sub IDT for
    * @param arguments the arguments passed from caller method
    * @param callerIndex the caller index
    * @param budget the budget for the sub IDT
    * @param callStack the call stack
    */
   void buildIDT2(TR::IDTNode* node, std::vector<TR::AbsValue*>* arguments, int32_t callerIndex, int32_t budget, TR_CallStack* callStack);
   
   /**
    * @brief add IDTNode(s) to the IDT
    *
    * @param parent the parent node to add children for
    * @param callerIndex the caller index
    * @param callSite the call site
    * @param callRatio the call ratio of this callsite
    * @param arguments the arguments passed from the caller method.
    * @param callStack the call stack
    * 
    * @return void
    */
   void addNodesToIDT(TR::IDTNode* parent, int32_t callerIndex, TR_CallSite* callSite, float callRatio, std::vector<TR::AbsValue*>* arguments, TR_CallStack* callStack);

   TR::IDTNode* checkIfMethodIsInterpreted(TR_ResolvedMethod* method);
   void storeInterpretedMethod(TR_ResolvedMethod* method, TR::IDTNode* node);
   
   uint32_t computeStaticBenefit(TR::InliningMethodSummary* summary, std::vector<TR::AbsValue*>* arguments);
   
   TR::IDT* _idt;
   TR::ResolvedMethodSymbol* _rootSymbol;

   typedef TR::typed_allocator<std::pair<TR_OpaqueMethodBlock*, TR::IDTNode*>, TR::Region&> InterpretedMethodMapAllocator;
   typedef std::less<TR_OpaqueMethodBlock*> InterpretedMethodMapComparator;
   std::map<TR_OpaqueMethodBlock *, TR::IDTNode*, InterpretedMethodMapComparator, InterpretedMethodMapAllocator> _interpretedMethodMap;

   int32_t _rootBudget;
   TR::Region& _region;
   TR::Compilation* _comp;
   TR_InlinerBase* _inliner;
   };
}

namespace TR {

class IDTBuilderVisitor : public TR::AbsVisitor
   {
   public:
   IDTBuilderVisitor(TR::IDTBuilder* idtBuilder, TR::IDTNode* idtNode, TR_CallStack* callStack) :
         _idtBuilder(idtBuilder),
         _idtNode(idtNode),
         _callStack(callStack)
      {}
      
   virtual void visitCallSite(TR_CallSite* callSite, int32_t callerIndex, TR::Block* callBlock, std::vector<TR::AbsValue*>* arguments);

   private:
   TR::IDTBuilder* _idtBuilder;
   TR::IDTNode* _idtNode;
   TR_CallStack* _callStack;
   };
}

#endif
