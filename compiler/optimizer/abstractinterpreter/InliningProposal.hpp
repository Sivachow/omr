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

#ifndef INLINING_PROPOSAL_INCL
#define INLINING_PROPOSAL_INCL

#include "env/Region.hpp"
#include "optimizer/abstractinterpreter/IDT.hpp"
#include "optimizer/abstractinterpreter/IDTNode.hpp"
#include "infra/BitVector.hpp" 
#include "compile/Compilation.hpp"

namespace TR {

/**
 *Inlining Proposal records the set of IDTNodes selected to be inlined.
 */
class InliningProposal
   {
   public:

   InliningProposal(TR::Region& region, TR::IDT *idt);
   InliningProposal(const TR::InliningProposal&, TR::Region& region);

   void print(TR::Compilation *comp);
   bool isEmpty();

   uint32_t getCost();
   uint64_t getBenefit();

   /**
    * @brief add an IDTNode selected to be inlined to the proposal
    *
    * @param node the IDTNode
    */
   void addNode(TR::IDTNode* node);

   /**
    * @brief Check if the node is selected to be inlined
    *
    * @param node the IDTNode to be checked
    * 
    * @return true if in proposal. false otherwise.
    */
   bool isNodeInProposal(TR::IDTNode* node );

   /**
    * @brief Union two proposals.
    *
    * @param a one proposal
    * @param b another proposal
    */
   void unionInPlace(TR::InliningProposal *a, TR::InliningProposal* b);

   /**
    * @brief Check if self intersects with another proposal.
    *
    * @param other the proposal
    * 
    * @return true of they intersect. false otherwise.
    */
   bool intersects(TR::InliningProposal* other);

   private:

   void computeCostAndBenefit();
   void ensureBitVectorInitialized();

   TR::Region& _region;
   TR_BitVector *_nodes;
   uint32_t _cost;
   uint64_t _benefit;
   TR::IDT *_idt;
};


class InliningProposalTable
   {
   public:

   InliningProposalTable(uint32_t rows, uint32_t cols, TR::Region& region);
   TR::InliningProposal* get(uint32_t row, uint32_t col);
   void set(uint32_t row, uint32_t col, TR::InliningProposal* proposal);

   TR::Region& region() { return _region; }

   TR::InliningProposal* getEmptyProposal() { return new (region()) TR::InliningProposal(region(), NULL); }
   
   private:
   
   uint32_t _rows;
   uint32_t _cols;
   TR::Region& _region;
   TR::InliningProposal ***_table;
   };
}

#endif
