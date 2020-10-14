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

#include <gtest/gtest.h>
#include <exception>
#include "../CodeGenTest.hpp"
#include "AbsInterpreterTest.hpp"
#include "optimizer/abstractinterpreter/AbsValue.hpp"
#include "optimizer/abstractinterpreter/AbsOpArray.hpp"
#include "optimizer/abstractinterpreter/AbsOpStack.hpp"

class AbsVPValueTest : public TRTest::AbsInterpreterTest {};

TEST_F(AbsVPValueTest, testParameter) {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   ASSERT_FALSE(value1.isParameter());
   value1.setParamPosition(1);
   ASSERT_TRUE(value1.isParameter());
}

TEST_F(AbsVPValueTest, testSetToTop) {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   ASSERT_FALSE(value1.isTop());
   value1.setToTop();
   ASSERT_TRUE(value1.isTop());
}

TEST_F(AbsVPValueTest, testDataType) {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   ASSERT_FALSE(value1.isTop());
   value1.setToTop();
   ASSERT_TRUE(value1.isTop());
}

TEST_F(AbsVPValueTest, testSetParam) {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   bool dataTypeEq = false;
   if (value1.getDataType() == TR::Int32)
      dataTypeEq = true;
   ASSERT_TRUE(dataTypeEq);
}

TEST_F(AbsVPValueTest, testCloneOperation) {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   TR::AbsVPValue* value2 = static_cast<TR::AbsVPValue*>(value1.clone(region()));
   ASSERT_EQ(value1.getConstraint(), value2->getConstraint());  
   bool dataTypeEq = false;
   if (value1.getDataType() == value2->getDataType())
      dataTypeEq = true;
   ASSERT_TRUE(dataTypeEq);
   ASSERT_EQ(value1.getParamPosition(), value2->getParamPosition());
}

class AbsOpStackTest : public TRTest::AbsInterpreterTest {};

TEST_F(AbsOpStackTest, testPushPopAndPeek) {
   TR::AbsOpStack stack;
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value2(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value3(vp(), &constraint, TR::Int32);
   stack.push(&value1);
   stack.push(&value2);
   stack.push(&value3);
   ASSERT_EQ(stack.size(), 3);
   ASSERT_EQ(stack.peek(),&value3);
   ASSERT_EQ(stack.pop(),&value3);
   ASSERT_EQ(stack.peek(),&value2);
   stack.pop();
   stack.pop();
   ASSERT_TRUE(stack.empty());
}


TEST_F(AbsOpStackTest, testCloneOperation) {
   TR::AbsOpStack stack1;
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value2(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value3(vp(), &constraint, TR::Int32);
   stack1.push(&value1);
   stack1.push(&value2);
   stack1.push(&value3);
  
   TR::AbsOpStack* stack2 = stack1.clone(region());
   ASSERT_EQ(stack2->size(), 3);
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(stack2->peek())->getConstraint(),value3.getConstraint());
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(stack2->peek())->getParamPosition(),value3.getParamPosition());
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(stack2->pop())->getConstraint(),value3.getConstraint());
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(stack2->peek())->getConstraint(),value2.getConstraint());
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(stack2->peek())->getParamPosition(),value2.getParamPosition());
   stack2->pop();
   stack2->pop();
   ASSERT_TRUE(stack2->empty());
}


class AbsOpArrayTest : public TRTest::AbsInterpreterTest {};

TEST_F(AbsOpArrayTest, testSetAndAt) {
   TR::AbsOpArray array(3);
   ASSERT_EQ(array.size(), 3);
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value2(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value3(vp(), &constraint, TR::Int32);
   array.set(0, &value1);
   array.set(1, &value2);
   array.set(2, &value3);
   ASSERT_EQ(array.at(0), &value1);
   ASSERT_EQ(array.at(1), &value2);
   ASSERT_EQ(array.at(2), &value3);
}


TEST_F(AbsOpArrayTest, testCloneOperation) {
   TR::AbsOpArray array(3);
   ASSERT_EQ(array.size(), 3);
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value2(vp(), &constraint, TR::Int32);
   TR::AbsVPValue value3(vp(), &constraint, TR::Int32);
   array.set(0, &value1);
   array.set(1, &value2);
   array.set(2, &value3);

   TR::AbsOpArray* array2 = array.clone(region());

   ASSERT_EQ(static_cast<TR::AbsVPValue*>(array.at(0))->getConstraint(), value1.getConstraint());
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(array.at(1))->getConstraint(), value2.getConstraint());
   ASSERT_EQ(static_cast<TR::AbsVPValue*>(array.at(2))->getConstraint(), value3.getConstraint());
}
