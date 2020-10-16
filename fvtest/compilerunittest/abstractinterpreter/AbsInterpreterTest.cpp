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
#include "../AbsInterpreterTest.hpp"
#include "optimizer/abstractinterpreter/AbsValue.hpp"
#include "optimizer/abstractinterpreter/AbsOpArray.hpp"
#include "optimizer/abstractinterpreter/AbsOpStack.hpp"


class AbsVPValueTest : public TRTest::AbsInterpreterTest {};

TEST_F(AbsVPValueTest, testParameter) 
   {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   ASSERT_FALSE(value1.isParameter());
   value1.setParameterPosition(1);
   ASSERT_TRUE(value1.isParameter());
   ASSERT_EQ(1, value1.getParameterPosition());
   }

TEST_F(AbsVPValueTest, testSetToTop)
   {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   ASSERT_FALSE(value1.isTop());
   value1.setToTop();
   ASSERT_TRUE(value1.isTop());
   }

TEST_F(AbsVPValueTest, testDataType) 
   {
   TR::AbsVPValue value1(vp(), NULL, TR::Int32);

   //Not able to test using ASSERT_EQ because the == operator of TR::DataType is not declared as const.
   ASSERT_TRUE(value1.getDataType() == TR::Int32);
   TR::AbsVPValue value2(vp(), NULL, TR::Int64);
   ASSERT_TRUE(value2.getDataType() == TR::Int64);
   TR::AbsVPValue value3(vp(), NULL, TR::Float);
   ASSERT_TRUE(value3.getDataType() == TR::Float);
   TR::AbsVPValue value4(vp(), NULL, TR::Double);
   ASSERT_TRUE(value4.getDataType() == TR::Double);
   TR::AbsVPValue value5(vp(), NULL, TR::Address);
   ASSERT_TRUE(value5.getDataType() == TR::Address);
   }

TEST_F(AbsVPValueTest, testSetParam)
   {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   ASSERT_EQ(-1, value1.getParameterPosition());
   value1.setParameterPosition(2);
   ASSERT_EQ(2, value1.getParameterPosition());
   value1.setParameterPosition(5);
   ASSERT_EQ(5, value1.getParameterPosition());
   }

TEST_F(AbsVPValueTest, testCloneOperation)
   {
   TR::VPIntConst constraint(1);
   TR::AbsVPValue value1(vp(), &constraint, TR::Int32);
   TR::AbsVPValue* value2 = static_cast<TR::AbsVPValue*>(value1.clone(region()));

   //the pointer of the cloned value should not be equal to the one being cloned.
   ASSERT_NE(&value1, value2);

   //the cloned value should be identical to the one cloned.
   ASSERT_EQ(value1.getConstraint(), value2->getConstraint());  
   ASSERT_TRUE(value1.getDataType() == value2->getDataType());
   ASSERT_EQ(value1.getParameterPosition(), value2->getParameterPosition());
   ASSERT_EQ(value1.isParameter(), value2->isParameter());
   ASSERT_EQ(value1.isTop(), value2->isTop());
   }

TEST_F(AbsVPValueTest, testMergeOperation)
   {
   TR::VPIntConst constraint1(1);
   TR::AbsVPValue value1(vp(), &constraint1, TR::Int32);
   
   TR::VPIntConst constraint2(2);
   TR::AbsVPValue value2(vp(), &constraint2, TR::Int32);

   TR::AbsVPValue* mergedValue1 = static_cast<TR::AbsVPValue*>(value1.merge(&value2));

   ASSERT_FALSE(mergedValue1->isTop());
   ASSERT_TRUE(mergedValue1->getConstraint()->asIntRange() != NULL);
   ASSERT_TRUE(mergedValue1->getDataType() == TR::Int32);
   ASSERT_EQ(1, mergedValue1->getConstraint()->getLowInt());
   ASSERT_EQ(2, mergedValue1->getConstraint()->getHighInt());

   TR::VPLongConst constraint3(1);
   TR::AbsVPValue value3(vp(), &constraint3, TR::Int64);
   
   TR::VPLongConst constraint4(3);
   TR::AbsVPValue value4(vp(), &constraint4, TR::Int64);

   TR::AbsVPValue* mergedValue2 = static_cast<TR::AbsVPValue*>(value3.merge(&value4));
   ASSERT_FALSE(mergedValue2->isTop());
   ASSERT_TRUE(mergedValue2->getDataType() == TR::Int64);
   ASSERT_TRUE(mergedValue2->getConstraint()->asMergedLongConstraints() != NULL);
   ASSERT_EQ(1, mergedValue2->getConstraint()->getLowLong());
   ASSERT_EQ(3, mergedValue2->getConstraint()->getHighLong());

   TR::VPIntRange constraint5(1, 4);
   TR::AbsVPValue value5(vp(), &constraint5, TR::Int32);
   
   TR::VPIntRange constraint6(2, 8);
   TR::AbsVPValue value6(vp(), &constraint6, TR::Int32);

   TR::AbsVPValue* mergedValue3 = static_cast<TR::AbsVPValue*>(value5.merge(&value6));
   ASSERT_FALSE(mergedValue3->isTop());
   ASSERT_TRUE(mergedValue3->getConstraint()->asIntRange() != NULL);
   ASSERT_EQ(1, mergedValue3->getConstraint()->getLowInt());
   ASSERT_EQ(8, mergedValue3->getConstraint()->getHighInt());

   TR::AbsVPValue value7(vp(), NULL, TR::Float);
   TR::AbsVPValue value8(vp(), NULL, TR::Float);
   TR::AbsVPValue* mergedValue4 = static_cast<TR::AbsVPValue*>(value7.merge(&value8));
   ASSERT_TRUE(mergedValue4->isTop());
   ASSERT_TRUE(mergedValue4->getDataType() == TR::Float);

   TR::AbsVPValue value9(vp(), NULL, TR::Int32);
   TR::AbsVPValue value10(vp(), NULL, TR::Float);
   TR::AbsVPValue* mergedValue5 = static_cast<TR::AbsVPValue*>(value9.merge(&value10));
   ASSERT_TRUE(mergedValue5->isTop());
   ASSERT_TRUE(mergedValue5->getDataType() == TR::NoType);

   TR::AbsVPValue value11(vp(), NULL, TR::Float);
   TR::AbsVPValue* mergedValue6 = static_cast<TR::AbsVPValue*>(value11.merge(NULL));
   ASSERT_EQ(&value11, mergedValue6);

   TR::AbsVPValue value12(vp(), NULL, TR::Float);
   TR::AbsVPValue value13(vp(), NULL, TR::Float);
   TR::AbsVPValue* mergedValue7 = static_cast<TR::AbsVPValue*>(value12.merge(&value13));
   ASSERT_EQ(-1, mergedValue7->getParameterPosition());

   TR::AbsVPValue value14(vp(), NULL, TR::Float);
   value14.setParameterPosition(3);
   TR::AbsVPValue value15(vp(), NULL, TR::Float);
   value15.setParameterPosition(3);
   TR::AbsVPValue* mergedValue8 = static_cast<TR::AbsVPValue*>(value14.merge(&value15));
   ASSERT_EQ(3, mergedValue8->getParameterPosition());

   TR::AbsVPValue value16(vp(), NULL, TR::Float);
   value14.setParameterPosition(3);
   TR::AbsVPValue value17(vp(), NULL, TR::Float);
   value15.setParameterPosition(4);
   TR::AbsVPValue* mergedValue9 = static_cast<TR::AbsVPValue*>(value16.merge(&value17));
   ASSERT_EQ(-1, mergedValue9->getParameterPosition());
   }

class AbsOpStackTest : public TRTest::AbsInterpreterTest {};
TEST_F(AbsOpStackTest, testSizeAndEmpty)
   {
   TR::AbsOpStack stack;
   ASSERT_EQ(0, stack.size());
   ASSERT_TRUE(stack.empty());
   
   stack.push(NULL);
   ASSERT_EQ(1, stack.size());
   ASSERT_FALSE(stack.empty());

   stack.push(NULL);
   ASSERT_EQ(2, stack.size());
   ASSERT_FALSE(stack.empty());

   stack.push(NULL);
   ASSERT_EQ(3, stack.size());
   ASSERT_FALSE(stack.empty());

   stack.peek();
   ASSERT_EQ(3, stack.size());
   ASSERT_FALSE(stack.empty());

   stack.pop();
   ASSERT_EQ(2, stack.size());
   ASSERT_FALSE(stack.empty());

   stack.pop();
   ASSERT_EQ(1, stack.size());
   ASSERT_FALSE(stack.empty());

   stack.pop();
   ASSERT_EQ(0, stack.size());
   ASSERT_TRUE(stack.empty());
   }

TEST_F(AbsOpStackTest, testPushPopAndPeek)
   {
   TR::AbsOpStack stack;

   TRTest::AbsTestValue value1(TR::Int32);
   TRTest::AbsTestValue value2(TR::Int32);
   TRTest::AbsTestValue value3(TR::Int32);

   // push and pop should correspond with each other
   stack.push(&value1);
   stack.push(&value2);
   stack.push(&value3);
   ASSERT_EQ(&value3, stack.peek());
   ASSERT_EQ(&value3, stack.pop());
   ASSERT_EQ(&value2, stack.peek());
   stack.pop();
   stack.pop();
   }

TEST_F(AbsOpStackTest, testCloneOperation)
   {
   TR::AbsOpStack stack1;

   TRTest::AbsTestValue value1(TR::Int32);
   TRTest::AbsTestValue value2(TR::Int32);
   TRTest::AbsTestValue value3(TR::Int32);

   stack1.push(&value1);
   stack1.push(&value2);
   stack1.push(&value3);

   TR::AbsOpStack* stack2 = stack1.clone(region());

   //clone does not affect stack1
   ASSERT_EQ(3, stack1.size());
   ASSERT_EQ(&value3, stack1.pop());
   ASSERT_EQ(&value2, stack1.pop());
   ASSERT_EQ(&value1, stack1.pop());

   //cloned stack should have different pointers of abstract values.
   ASSERT_EQ(3, stack2->size());

   ASSERT_NE(&value3, stack2->peek());
   stack2->pop();

   ASSERT_NE(&value2, stack2->peek());
   stack2->pop();

   ASSERT_NE(&value1, stack2->peek());
   stack2->pop();
   }

TEST_F(AbsOpStackTest, testMergeOperation)
   {
   TR::AbsOpStack stack1;

   TRTest::AbsTestValue value1(TR::Int32);
   TRTest::AbsTestValue value2(TR::Int32);
   TRTest::AbsTestValue value3(TR::Int32);

   stack1.push(&value1);
   stack1.push(&value2);
   stack1.push(&value3);
   
   TR::AbsOpStack stack2;

   stack2.push(&value1);
   stack2.push(&value2);
   stack2.push(&value3);

   stack1.merge(&stack2);

   //merged stack size should not be changed.
   ASSERT_EQ(3, stack1.size());

   //other should not be affected
   ASSERT_EQ(3, stack2.size());
   ASSERT_EQ(&value3, stack2.pop());
   ASSERT_EQ(&value2, stack2.pop());
   ASSERT_EQ(&value1, stack2.pop());
   }


class AbsOpArrayTest : public TRTest::AbsInterpreterTest {};

TEST_F(AbsOpArrayTest, testSize) 
   {
   TR::AbsOpArray array(3);
   ASSERT_EQ(3, array.size());

   TR::AbsOpArray array2(5);
   ASSERT_EQ(5, array2.size());
   }

TEST_F(AbsOpArrayTest, testSetAndAt) 
   {
   TR::AbsOpArray array(3);

   TRTest::AbsTestValue value1(TR::Int32);
   TRTest::AbsTestValue value2(TR::Int32);
   TRTest::AbsTestValue value3(TR::Int32);

   array.set(0, &value1);
   array.set(1, &value2);
   array.set(2, &value3);
   ASSERT_EQ(&value1, array.at(0));
   ASSERT_EQ(&value2, array.at(1));
   ASSERT_EQ(&value3, array.at(2));
   }


TEST_F(AbsOpArrayTest, testCloneOperation) {
   TR::AbsOpArray array(3);

   TRTest::AbsTestValue value1(TR::Int32);
   TRTest::AbsTestValue value2(TR::Int32);
   TRTest::AbsTestValue value3(TR::Int32);

   array.set(0, &value1);
   array.set(1, &value2);
   array.set(2, &value3);

   TR::AbsOpArray* array2 = array.clone(region());

   //the array being cloned should not be affected.
   ASSERT_EQ(3, array.size());
   ASSERT_EQ(&value1, array.at(0));
   ASSERT_EQ(&value2, array.at(1));
   ASSERT_EQ(&value3, array.at(2));

   //cloned array should have different abstract value pointers.
   ASSERT_EQ(3, array2->size());
   ASSERT_NE(&value1, array2->at(0));
   ASSERT_NE(&value2, array2->at(1));
   ASSERT_NE(&value3, array2->at(2));
}

TEST_F(AbsOpArrayTest, testMergeOperation) 
   {
   TR::AbsOpArray array(3);

   TRTest::AbsTestValue value1(TR::Int32);
   TRTest::AbsTestValue value2(TR::Int32);
   TRTest::AbsTestValue value3(TR::Int32);

   array.set(0, &value1);
   array.set(1, &value2);
   array.set(2, &value3);

   TR::AbsOpArray array2(3);

   TRTest::AbsTestValue value4(TR::Int32);
   TRTest::AbsTestValue value5(TR::Int32);
   TRTest::AbsTestValue value6(TR::Int32);

   array2.set(0, &value4);
   array2.set(1, &value5);
   array2.set(2, &value6);

   array.merge(&array2);

   //merged array size should not be changed.
   ASSERT_EQ(3, array.size());

   //other should not be affected
   ASSERT_EQ(3, array2.size());
   ASSERT_EQ(&value4, array2.at(0));
   ASSERT_EQ(&value5, array2.at(1));
   ASSERT_EQ(&value6, array2.at(2));
   }

