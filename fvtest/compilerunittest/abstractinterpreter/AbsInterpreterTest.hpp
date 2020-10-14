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

#ifndef ABS_INTERPRETER_HPP
#define ABS_INTERPRETER_HPP

#include <gtest/gtest.h>
#include <exception>
#include "../CodeGenTest.hpp"
#include "env/SystemSegmentProvider.hpp"
#include "env/Region.hpp"
#include "compile/Compilation.hpp"
#include "compile/Compilation_inlines.hpp"
#include "optimizer/GlobalValuePropagation.hpp"



namespace TRTest {


class AbsInterpreterTest : public ::testing::Test
   {
   public:
   AbsInterpreterTest() :
      _jitInit(),
      _rawAllocator(),
      _segmentProvider(1 << 16, _rawAllocator),
      _dispatchRegion(_segmentProvider, _rawAllocator),
      _trMemory(*OMR::FrontEnd::singleton().persistentMemory(), _dispatchRegion),
      _types(),
      _options(),
      _ilGenRequest(),
      _method("compunittest", "0", "test", 0, NULL, _types.NoType, NULL, NULL),
      _comp(0, NULL, &OMR::FrontEnd::singleton(), &_method, _ilGenRequest, _options, _dispatchRegion, &_trMemory, TR_OptimizationPlan::alloc(warm)),
      _optimizer(&_comp, NULL, false),
      _manager(&_optimizer, TR::GlobalValuePropagation::create, OMR::globalValuePropagation),
      _vp(&_manager)
      {}


   TR::ValuePropagation* vp() { return &_vp; }
   TR::Region& region() { return _dispatchRegion; }

   private:
   JitInitializer _jitInit;

   TR::RawAllocator _rawAllocator;
   TR::SystemSegmentProvider _segmentProvider;
   TR::Region _dispatchRegion;
   TR_Memory _trMemory;

   TR::TypeDictionary _types;

   TR::Options _options;
   NullIlGenRequest _ilGenRequest;
   TR::ResolvedMethod _method;
   TR::Compilation _comp;

   TR::Optimizer _optimizer;
   TR::OptimizationManager _manager;

   TR::GlobalValuePropagation _vp;

   };
}
#endif
