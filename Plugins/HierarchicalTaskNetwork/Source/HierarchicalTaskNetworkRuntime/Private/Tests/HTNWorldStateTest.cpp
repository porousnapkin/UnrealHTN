// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "HTNWorldStateStruct.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHTNWorldStateTest, "HTNPlanner.WorldState", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FHTNWorldStateTest::RunTest(const FString& Parameters)
{
	// Test creating and getting properties
	{
		FHTNWorldStateStruct WorldState;
		
		// Set different property types
		WorldState.SetProperty(FName("BoolProp"), FHTNProperty(true));
		WorldState.SetProperty(FName("IntProp"), FHTNProperty(42));
		WorldState.SetProperty(FName("FloatProp"), FHTNProperty(3.14f));
		WorldState.SetProperty(FName("StringProp"), FHTNProperty(FString("Test String")));
		WorldState.SetProperty(FName("NameProp"), FHTNProperty(FName("TestName")));
		WorldState.SetProperty(FName("VectorProp"), FHTNProperty(FVector(1.0f, 2.0f, 3.0f)));
		
		// Test HasProperty
		TestTrue("Has boolean property", WorldState.HasProperty(FName("BoolProp")));
		TestTrue("Has integer property", WorldState.HasProperty(FName("IntProp")));
		TestTrue("Has float property", WorldState.HasProperty(FName("FloatProp")));
		TestTrue("Has string property", WorldState.HasProperty(FName("StringProp")));
		TestTrue("Has name property", WorldState.HasProperty(FName("NameProp")));
		TestTrue("Has vector property", WorldState.HasProperty(FName("VectorProp")));
		TestFalse("Doesn't have nonexistent property", WorldState.HasProperty(FName("NonexistentProp")));
		
		// Test GetProperty
		{
			FHTNProperty Value;
			TestTrue("GetProperty returns true for existing property", WorldState.GetProperty(FName("BoolProp"), Value));
			TestEqual("GetProperty retrieves correct boolean value", Value.GetBoolValue(), true);
			
			TestTrue("GetProperty returns true for existing property", WorldState.GetProperty(FName("IntProp"), Value));
			TestEqual("GetProperty retrieves correct integer value", Value.GetIntValue(), 42);
			
			TestTrue("GetProperty returns true for existing property", WorldState.GetProperty(FName("FloatProp"), Value));
			TestEqual("GetProperty retrieves correct float value", Value.GetFloatValue(), 3.14f);
			
			TestTrue("GetProperty returns true for existing property", WorldState.GetProperty(FName("StringProp"), Value));
			TestEqual("GetProperty retrieves correct string value", Value.GetStringValue(), FString("Test String"));
			
			TestTrue("GetProperty returns true for existing property", WorldState.GetProperty(FName("NameProp"), Value));
			TestEqual("GetProperty retrieves correct name value", Value.GetNameValue(), FName("TestName"));
			
			TestTrue("GetProperty returns true for existing property", WorldState.GetProperty(FName("VectorProp"), Value));
			TestTrue("GetProperty retrieves correct vector value", Value.GetVectorValue().Equals(FVector(1.0f, 2.0f, 3.0f)));
			
			TestFalse("GetProperty returns false for nonexistent property", WorldState.GetProperty(FName("NonexistentProp"), Value));
		}
		
		// Test template accessor methods
		TestEqual("GetPropertyValue<bool> retrieves correct value", WorldState.GetPropertyValue<bool>(FName("BoolProp"), false), true);
		TestEqual("GetPropertyValue<int32> retrieves correct value", WorldState.GetPropertyValue<int32>(FName("IntProp"), 0), 42);
		TestEqual("GetPropertyValue<float> retrieves correct value", WorldState.GetPropertyValue<float>(FName("FloatProp"), 0.0f), 3.14f);
		TestEqual("GetPropertyValue<FString> retrieves correct value", WorldState.GetPropertyValue<FString>(FName("StringProp"), FString()), FString("Test String"));
		TestEqual("GetPropertyValue<FName> retrieves correct value", WorldState.GetPropertyValue<FName>(FName("NameProp"), FName()), FName("TestName"));
		TestTrue("GetPropertyValue<FVector> retrieves correct value", WorldState.GetPropertyValue<FVector>(FName("VectorProp"), FVector::ZeroVector).Equals(FVector(1.0f, 2.0f, 3.0f)));
		
		// Test default values for nonexistent properties
		TestEqual("GetPropertyValue returns default for nonexistent property", WorldState.GetPropertyValue<bool>(FName("NonexistentBool"), true), true);
		TestEqual("GetPropertyValue returns default for nonexistent property", WorldState.GetPropertyValue<int32>(FName("NonexistentInt"), 123), 123);
		TestEqual("GetPropertyValue returns default for nonexistent property", WorldState.GetPropertyValue<float>(FName("NonexistentFloat"), 2.71f), 2.71f);
		TestEqual("GetPropertyValue returns default for nonexistent property", WorldState.GetPropertyValue<FString>(FName("NonexistentString"), FString("Default")), FString("Default"));
		TestEqual("GetPropertyValue returns default for nonexistent property", WorldState.GetPropertyValue<FName>(FName("NonexistentName"), FName("Default")), FName("Default"));
		TestTrue("GetPropertyValue returns default for nonexistent property", WorldState.GetPropertyValue<FVector>(FName("NonexistentVector"), FVector(4.0f, 5.0f, 6.0f)).Equals(FVector(4.0f, 5.0f, 6.0f)));
	}
	
	// Test world state cloning
	{
		FHTNWorldStateStruct Original;
		Original.SetProperty(FName("BoolProp"), FHTNProperty(true));
		Original.SetProperty(FName("IntProp"), FHTNProperty(42));
		
		FHTNWorldStateStruct Clone = Original.Clone();
		
		// Verify clone has the same properties
		TestTrue("Clone has BoolProp", Clone.HasProperty(FName("BoolProp")));
		TestTrue("Clone has IntProp", Clone.HasProperty(FName("IntProp")));
		
		FHTNProperty BoolValue, IntValue;
		Clone.GetProperty(FName("BoolProp"), BoolValue);
		Clone.GetProperty(FName("IntProp"), IntValue);
		
		TestEqual("Cloned BoolProp has correct value", BoolValue.GetBoolValue(), true);
		TestEqual("Cloned IntProp has correct value", IntValue.GetIntValue(), 42);
		
		// Modify clone and verify it doesn't affect original
		Clone.SetProperty(FName("BoolProp"), FHTNProperty(false));
		Clone.SetProperty(FName("NewProp"), FHTNProperty(FString("New Property")));
		
		FHTNProperty OriginalBoolValue;
		Original.GetProperty(FName("BoolProp"), OriginalBoolValue);
		
		TestEqual("Original BoolProp not affected by clone change", OriginalBoolValue.GetBoolValue(), true);
		TestFalse("Original doesn't have new property from clone", Original.HasProperty(FName("NewProp")));
	}
	
	// Test world state comparison
	{
		FHTNWorldStateStruct State1;
		State1.SetProperty(FName("Prop1"), FHTNProperty(42));
		State1.SetProperty(FName("Prop2"), FHTNProperty(FString("Value")));
		
		FHTNWorldStateStruct State2;
		State2.SetProperty(FName("Prop1"), FHTNProperty(42));
		State2.SetProperty(FName("Prop2"), FHTNProperty(FString("Value")));
		
		FHTNWorldStateStruct State3;
		State3.SetProperty(FName("Prop1"), FHTNProperty(42));
		State3.SetProperty(FName("Prop2"), FHTNProperty(FString("Different")));
		
		FHTNWorldStateStruct State4;
		State4.SetProperty(FName("Prop1"), FHTNProperty(42));
		
		TestTrue("Equal world states compare equal", State1.Equals(State2));
		TestTrue("Equal world states operator== returns true", State1 == State2);
		
		TestFalse("Different world states compare not equal", State1.Equals(State3));
		TestTrue("Different world states operator!= returns true", State1 != State3);
		
		TestFalse("Different property count compare not equal", State1.Equals(State4));
	}
	
	// Test world state difference
	{
		FHTNWorldStateStruct State1;
		State1.SetProperty(FName("SharedSame"), FHTNProperty(42));
		State1.SetProperty(FName("SharedDifferent"), FHTNProperty(true));
		State1.SetProperty(FName("OnlyInState1"), FHTNProperty(FString("State1 Only")));
		
		FHTNWorldStateStruct State2;
		State2.SetProperty(FName("SharedSame"), FHTNProperty(42));
		State2.SetProperty(FName("SharedDifferent"), FHTNProperty(false));
		State2.SetProperty(FName("OnlyInState2"), FHTNProperty(FString("State2 Only")));
		
		FHTNWorldStateStruct Difference = State1.CreateDifference(State2);
		
		// Difference should contain SharedDifferent, OnlyInState1, and OnlyInState2
		TestEqual("Difference has correct number of properties", Difference.GetPropertyNames().Num(), 3);
		TestFalse("Difference doesn't contain properties that are the same", Difference.HasProperty(FName("SharedSame")));
		TestTrue("Difference contains properties with different values", Difference.HasProperty(FName("SharedDifferent")));
		TestTrue("Difference contains properties only in State1", Difference.HasProperty(FName("OnlyInState1")));
		TestTrue("Difference contains properties only in State2", Difference.HasProperty(FName("OnlyInState2")));
		
		// Verify property values in difference
		FHTNProperty DiffValue;
		Difference.GetProperty(FName("SharedDifferent"), DiffValue);
		TestEqual("Value from State1 is preserved in difference", DiffValue.GetBoolValue(), true);
		
		Difference.GetProperty(FName("OnlyInState1"), DiffValue);
		TestEqual("Value from State1 is preserved in difference", DiffValue.GetStringValue(), FString("State1 Only"));
		
		Difference.GetProperty(FName("OnlyInState2"), DiffValue);
		TestEqual("Value from State2 is preserved in difference", DiffValue.GetStringValue(), FString("State2 Only"));
	}
	
	// Test property removal
	{
		FHTNWorldStateStruct WorldState;
		WorldState.SetProperty(FName("PropToRemove"), FHTNProperty(42));
		
		TestTrue("HasProperty returns true before removal", WorldState.HasProperty(FName("PropToRemove")));
		
		TestTrue("RemoveProperty returns true for existing property", WorldState.RemoveProperty(FName("PropToRemove")));
		TestFalse("HasProperty returns false after removal", WorldState.HasProperty(FName("PropToRemove")));
		
		TestFalse("RemoveProperty returns false for nonexistent property", WorldState.RemoveProperty(FName("NonexistentProp")));
	}
	
	// Test getting property names
	{
		FHTNWorldStateStruct WorldState;
		WorldState.SetProperty(FName("Prop1"), FHTNProperty(true));
		WorldState.SetProperty(FName("Prop2"), FHTNProperty(42));
		WorldState.SetProperty(FName("Prop3"), FHTNProperty(3.14f));
		
		TArray<FName> PropertyNames = WorldState.GetPropertyNames();
		TestEqual("GetPropertyNames returns correct count", PropertyNames.Num(), 3);
		TestTrue("GetPropertyNames includes Prop1", PropertyNames.Contains(FName("Prop1")));
		TestTrue("GetPropertyNames includes Prop2", PropertyNames.Contains(FName("Prop2")));
		TestTrue("GetPropertyNames includes Prop3", PropertyNames.Contains(FName("Prop3")));
	}
	
	// Test ToString method
	{
		FHTNWorldStateStruct WorldState;
		WorldState.SetProperty(FName("BoolProp"), FHTNProperty(true));
		WorldState.SetProperty(FName("IntProp"), FHTNProperty(42));
		
		FString StateString = WorldState.ToString();
		TestTrue("ToString contains property name BoolProp", StateString.Contains(TEXT("BoolProp")));
		TestTrue("ToString contains boolean value", StateString.Contains(TEXT("true")));
		TestTrue("ToString contains property name IntProp", StateString.Contains(TEXT("IntProp")));
		TestTrue("ToString contains integer value", StateString.Contains(TEXT("42")));
	}
	
	// Test UHTNWorldState wrapper
	{
		UHTNWorldState* WorldState = NewObject<UHTNWorldState>();
		
		TestNotNull("Interface object is valid", WorldState);
		
		// Test basic operations
		WorldState->SetProperty(FName("TestProp"), FHTNProperty(42));
		TestTrue("Interface HasProperty works", WorldState->HasProperty(FName("TestProp")));
		
		FHTNProperty Value;
		TestTrue("Interface GetProperty works", WorldState->GetProperty(FName("TestProp"), Value));
		TestEqual("Interface property value is correct", Value.GetIntValue(), 42);
		
		// Test cloning
		UHTNWorldState* Clone = WorldState->Clone();
		TestNotNull("Cloned interface object is valid", Clone);
		
		FHTNProperty ClonedValue;
		TestTrue("Cloned interface has property", Clone->HasProperty(FName("TestProp")));
		TestTrue("Cloned interface GetProperty works", Clone->GetProperty(FName("TestProp"), ClonedValue));
		TestEqual("Cloned interface property value is correct", ClonedValue.GetIntValue(), 42);
		
		// Test equality
		TestTrue("Interface equals itself", WorldState->Equals(WorldState));
		TestTrue("Interface equals its clone", WorldState->Equals(Clone));
		
		// Test difference
		WorldState->SetProperty(FName("DiffProp"), FHTNProperty(true));
		UHTNWorldState* Difference = WorldState->CreateDifference(Clone);
		
		TestTrue("Difference has property only in first", Difference->HasProperty(FName("DiffProp")));
		TestFalse("Difference doesn't have shared property", Difference->HasProperty(FName("TestProp")));
	}
	
	// Test CreateFromStruct
	{
		FHTNWorldStateStruct StructState;
		StructState.SetProperty(FName("TestProp"), FHTNProperty(42));
		
		UHTNWorldState* ObjectState = UHTNWorldState::CreateFromStruct(StructState);
		TestNotNull("CreateFromStruct returns valid object", ObjectState);
		
		FHTNProperty Value;
		TestTrue("Created object has property", ObjectState->GetProperty(FName("TestProp"), Value));
		TestEqual("Created object property value is correct", Value.GetIntValue(), 42);
	}
	
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS