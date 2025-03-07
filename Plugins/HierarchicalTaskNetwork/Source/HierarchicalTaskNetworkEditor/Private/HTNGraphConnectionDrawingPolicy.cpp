// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphConnectionDrawingPolicy.h"
#include "HTNGraphSchema.h"

FHTNGraphConnectionDrawingPolicy::FHTNGraphConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
	, GraphObj(InGraphObj)
	, ZoomFactor(InZoomFactor)
{
}

void FHTNGraphConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;
	
	// Get schema
	const UHTNGraphSchema* Schema = Cast<UHTNGraphSchema>(GraphObj->GetSchema());
	if (Schema == nullptr)
	{
		Schema = GetDefault<UHTNGraphSchema>();
	}
	
	// Determine connection color based on pin type
	if (OutputPin && InputPin)
	{
		if (OutputPin->PinType.PinCategory == UHTNGraphSchema::PC_Task)
		{
			Params.WireColor = Schema->GetPinTypeColor(OutputPin->PinType);
		}
		else if (InputPin->PinType.PinCategory == UHTNGraphSchema::PC_Task)
		{
			Params.WireColor = Schema->GetPinTypeColor(InputPin->PinType);
		}
		else if (OutputPin->PinType.PinCategory == UHTNGraphSchema::PC_Sequence)
		{
			Params.WireColor = Schema->GetPinTypeColor(OutputPin->PinType);
			Params.bUserFlag1 = true; // Use a different style for sequence connections
		}
		else if (InputPin->PinType.PinCategory == UHTNGraphSchema::PC_Sequence)
		{
			Params.WireColor = Schema->GetPinTypeColor(InputPin->PinType);
			Params.bUserFlag1 = true; // Use a different style for sequence connections
		}
		else if (OutputPin->PinType.PinCategory == UHTNGraphSchema::PC_Condition)
		{
			Params.WireColor = Schema->GetPinTypeColor(OutputPin->PinType);
			Params.bUserFlag2 = true; // Use a different style for condition connections
		}
		else if (InputPin->PinType.PinCategory == UHTNGraphSchema::PC_Condition)
		{
			Params.WireColor = Schema->GetPinTypeColor(InputPin->PinType);
			Params.bUserFlag2 = true; // Use a different style for condition connections
		}
		else if (OutputPin->PinType.PinCategory == UHTNGraphSchema::PC_Effect)
		{
			Params.WireColor = Schema->GetPinTypeColor(OutputPin->PinType);
			Params.WireThickness = 1.0f; // Different thickness for effect connections
		}
		else if (InputPin->PinType.PinCategory == UHTNGraphSchema::PC_Effect)
		{
			Params.WireColor = Schema->GetPinTypeColor(InputPin->PinType);
			Params.WireThickness = 1.0f; // Different thickness for effect connections
		}
	}
	
	// Use the default drawing style unless it's a special connection
	const bool bStyleModified = Params.bUserFlag1 || Params.bUserFlag2;
	
	if (bStyleModified)
	{
		Params.WireThickness = Params.bUserFlag1 ? 2.0f : 1.5f;
		Params.bDrawBubbles = Params.bUserFlag2;
	}
}

void FHTNGraphConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	// Draw all regular connections
	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FHTNGraphConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	// Get the start and end points
	const FVector2D StartPoint = FGeometryHelper::VerticalMiddleLeftOf(StartGeom);
	const FVector2D EndPoint = FGeometryHelper::VerticalMiddleRightOf(EndGeom);
	
	DrawSplineWithArrow(StartPoint, EndPoint, Params);
}

void FHTNGraphConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params)
{
	// Determine if we need a special drawing style
	if (Params.bUserFlag1)
	{
		// Sequence connections: Thicker with arrows
		const FVector2D Tangent = ComputeSplineTangent(StartPoint, EndPoint);
		
		// Draw main line
		DrawConnection(WireLayerID, StartPoint, EndPoint, Params);
		
		// Draw a little arrow at the end
		const FVector2D ArrowDrawPos = EndPoint - Tangent.GetSafeNormal() * 10.0f;
		const float AngleInRadians = FMath::Atan2(Tangent.Y, Tangent.X);
		
		// Draw the arrow
		FSlateDrawElement::MakeRotatedBox(
			DrawElementsList,
			ArrowLayerID,
			FPaintGeometry(ArrowDrawPos, FVector2D(10.0f, 6.0f), ZoomFactor),
			FAppStyle::GetBrush(TEXT("Graph.Arrow")),
			ESlateDrawEffect::None,
			AngleInRadians
		);
	}
	else if (Params.bUserFlag2)
	{
		// Condition connections: Dotted line
		const float DashSize = 5.0f;
		const float GapSize = 5.0f;
		const FVector2D Direction = (EndPoint - StartPoint).GetSafeNormal();
		const float Distance = (EndPoint - StartPoint).Size();

		// Calculate how many dashes we need
		int32 NumDashes = FMath::FloorToInt(Distance / (DashSize + GapSize));
		if (NumDashes == 0) NumDashes = 1;  // At least one dash

		// Draw the dashes
		for (int32 i = 0; i < NumDashes; ++i)
		{
			const float StartDistance = i * (DashSize + GapSize);
			const float EndDistance = StartDistance + DashSize;
    
			// Make sure we don't go beyond the total distance
			if (StartDistance >= Distance) break;
    
			// Calculate segment start and end points
			const FVector2D SegmentStart = StartPoint + Direction * StartDistance;
			const FVector2D SegmentEnd = StartPoint + Direction * FMath::Min(EndDistance, Distance);
    
			// Draw the dash segment
			FSlateDrawElement::MakeBox(
				DrawElementsList,
				WireLayerID,
				FPaintGeometry(),
				nullptr,  // Use solid color instead of brush
				ESlateDrawEffect::None,
				Params.WireColor
			);
    
			// We can use a simpler approach by just drawing a line segment
			TArray<FVector2D> LinePoints;
			LinePoints.Add(SegmentStart);
			LinePoints.Add(SegmentEnd);
    
			FSlateDrawElement::MakeLines(
				DrawElementsList,
				WireLayerID,
				FPaintGeometry(),
				LinePoints,
				ESlateDrawEffect::None,
				Params.WireColor,
				true,  // Anti-alias
				Params.WireThickness
			);
		}
	}
	else if (Params.WireThickness == 1.0f) // This is the special thickness we set for effect connections
	{
		// Effect connections: Wavy line
		const FVector2D Tangent = ComputeSplineTangent(StartPoint, EndPoint);
		const float Distance = (EndPoint - StartPoint).Size();
		const int32 NumSteps = FMath::Max(8, FMath::FloorToInt(Distance / 20.0f));
		
		TArray<FVector2D> Points;
		Points.Reserve(NumSteps + 1);
		
		for (int32 Step = 0; Step <= NumSteps; ++Step)
		{
			const float Alpha = (float)Step / (float)NumSteps;
			const FVector2D PointOnLine = StartPoint + Alpha * (EndPoint - StartPoint);
			const float WaveOffset = FMath::Sin(Alpha * 6.0f * PI) * 10.0f;
			
			const FVector2D Normal = FVector2D(-Tangent.Y, Tangent.X).GetSafeNormal();
			const FVector2D WavePoint = PointOnLine + Normal * WaveOffset;
			
			Points.Add(WavePoint);
		}
		
		// Draw the wavy line
		FSlateDrawElement::MakeLines(
			DrawElementsList,
			WireLayerID,
			FPaintGeometry(),
			Points,
			ESlateDrawEffect::None,
			Params.WireColor,
			true, // bAntiAlias
			Params.WireThickness
		);
	}
	else
	{
		// Regular connections
		FConnectionDrawingPolicy::DrawSplineWithArrow(StartPoint, EndPoint, Params);
	}
}

void FHTNGraphConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, /*inout*/ Params);
	
	// Draw preview connector
	DrawSplineWithArrow(StartPoint, EndPoint, Params);
}

FVector2D FHTNGraphConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const
{
	// Adjusted tangent calculation for curve aesthetics
	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();
	
	return NormDelta;
}

void FHTNGraphConnectionDrawingPolicy::DetermineLinkGeometry(FArrangedChildren& ArrangedNodes, TSharedRef<SWidget>& OutputPinWidget, UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FArrangedWidget*& StartWidgetGeometry, FArrangedWidget*& EndWidgetGeometry)
{
	FConnectionDrawingPolicy::DetermineLinkGeometry(ArrangedNodes, OutputPinWidget, OutputPin, InputPin, StartWidgetGeometry, EndWidgetGeometry);
}