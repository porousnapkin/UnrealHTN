// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "HTNProperty.h"
// Include the concrete world state implementation when ready

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHTNPropertyTest, "HTNPlanner.Property", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FHTNPropertyTest::RunTest(const FString& Parameters)
{
	// Test boolean property
	{
		FHTNProperty BoolProp(true);
		TestEqual("Boolean property type", BoolProp.GetType(), EHTNPropertyType::Boolean);
		TestEqual("Boolean value", BoolProp.GetBoolValue(), true);
		TestEqual("Boolean to string", BoolProp.ToString(), TEXT("true"));
	}

	// Test integer property
	{
		FHTNProperty IntProp(42);
		TestEqual("Integer property type", IntProp.GetType(), EHTNPropertyType::Integer);
		TestEqual("Integer value", IntProp.GetIntValue(), 42);
		TestEqual("Integer to string", IntProp.ToString(), TEXT("42"));
	}

	// Test float property
	{
		FHTNProperty FloatProp(3.14f);
		TestEqual("Float property type", FloatProp.GetType(), EHTNPropertyType::Float);
		TestEqual("Float value", FloatProp.GetFloatValue(), 3.14f);
		// Float to string may have platform-specific formatting, so we just check it's not empty
		TestTrue("Float to string not empty", !FloatProp.ToString().IsEmpty());
	}

	// Test string property
	{
		FString TestString = TEXT("Test String");
		FHTNProperty StringProp(TestString);
		TestEqual("String property type", StringProp.GetType(), EHTNPropertyType::String);
		TestEqual("String value", StringProp.GetStringValue(), TestString);
		TestEqual("String to string", StringProp.ToString(), TestString);
	}

	// Test name property
	{
		FName ThisTestName = FName(TEXT("TestName"));
		FHTNProperty NameProp(ThisTestName);
		TestEqual("Name property type", NameProp.GetType(), EHTNPropertyType::Name);
		TestEqual("Name value", NameProp.GetNameValue(), ThisTestName);
		TestEqual("Name to string", NameProp.ToString(), ThisTestName.ToString());
	}

	// Test object property (using null for simplicity)
	{
		UObject* TestObject = nullptr;
		FHTNProperty ObjectProp(TestObject);
		TestEqual("Object property type", ObjectProp.GetType(), EHTNPropertyType::Object);
		TestEqual("Object value", ObjectProp.GetObjectValue(), TestObject);
		TestEqual("Object to string", ObjectProp.ToString(), TEXT("None"));
	}

	// Test vector property
	{
		FVector TestVector(1.0f, 2.0f, 3.0f);
		FHTNProperty VectorProp(TestVector);
		TestEqual("Vector property type", VectorProp.GetType(), EHTNPropertyType::Vector);
		TestTrue("Vector value equals", VectorProp.GetVectorValue().Equals(TestVector));
		// Vector to string may have platform-specific formatting, so we just check it's not empty
		TestTrue("Vector to string not empty", !VectorProp.ToString().IsEmpty());
	}

	// Test copy construction
	{
		FHTNProperty Original(42);
		FHTNProperty Copy(Original);
		TestEqual("Copy type equals original", Copy.GetType(), Original.GetType());
		TestEqual("Copy value equals original", Copy.GetIntValue(), Original.GetIntValue());
		TestTrue("Copy equals original", Copy == Original);
	}

	// Test move construction
	{
		FHTNProperty Original(FString("Move Test"));
		FString OriginalValue = Original.GetStringValue();
		FHTNProperty Moved(MoveTemp(Original));
		TestEqual("Moved type", Moved.GetType(), EHTNPropertyType::String);
		TestEqual("Moved value", Moved.GetStringValue(), OriginalValue);
		TestEqual("Original type after move", Original.GetType(), EHTNPropertyType::Invalid);
	}

	// Test equality operators
	{
		FHTNProperty Prop1(true);
		FHTNProperty Prop2(true);
		FHTNProperty Prop3(false);
		FHTNProperty Prop4(42);

		TestTrue("Equal properties compare equal", Prop1 == Prop2);
		TestTrue("Different value properties compare not equal", Prop1 != Prop3);
		TestTrue("Different type properties compare not equal", Prop1 != Prop4);
	}

	// Test type conversion behavior
	{
		FHTNProperty BoolProp(true);
		TestEqual("Bool to int", BoolProp.GetIntValue(), 1);
		TestEqual("Bool to float", BoolProp.GetFloatValue(), 1.0f);

		FHTNProperty IntProp(42);
		TestEqual("Int to bool (true)", IntProp.GetBoolValue(), true);
		TestEqual("Int to float", IntProp.GetFloatValue(), 42.0f);

		FHTNProperty ZeroIntProp(0);
		TestEqual("Zero int to bool (false)", ZeroIntProp.GetBoolValue(), false);
	}

	return true;
}

// Add more tests for the world state implementation when ready

#endif // WITH_DEV_AUTOMATION_TESTS