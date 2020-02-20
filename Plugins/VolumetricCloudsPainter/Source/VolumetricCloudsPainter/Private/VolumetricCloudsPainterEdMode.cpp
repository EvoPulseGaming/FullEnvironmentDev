// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VolumetricCloudsPainterEdMode.h"
#include "VolumetricCloudsPainterEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

#include "Runtime/Engine/Classes/Engine/Selection.h"

#include "Editor/UnrealEd/Public/EditorViewportClient.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"

#include "Runtime/Engine/Public/CanvasTypes.h"

#include "SlateBasics.h" 
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Editor/LevelEditor/Public/SLevelViewport.h"

#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"

#include "Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"

#include "FileHelpers.h"
#include "UObject/Package.h"
#include "AssetRegistryModule.h"



#include "EngineUtils.h"


const FEditorModeID FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId = TEXT("EM_VolumetricCloudsPainterEdMode");

FVolumetricCloudsPainterEdMode::FVolumetricCloudsPainterEdMode()
{
	ColorBlendMaterialInstance = Cast<UMaterialInstanceConstant>(StaticLoadObject(UMaterialInstanceConstant::StaticClass(), NULL,
		TEXT("MaterialInstanceConstant'/VolumetricCloudsPainter/Materials/MI_ColorBlend.MI_ColorBlend'")));
	AlphaBlendMaterialInstance = Cast<UMaterialInstanceConstant>(StaticLoadObject(UMaterialInstanceConstant::StaticClass(), NULL,
		TEXT("MaterialInstanceConstant'/VolumetricCloudsPainter/Materials/MI_AlphaBlend.MI_AlphaBlend'")));

	AlphCombineMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
		TEXT("Material'/VolumetricCloudsPainter/Materials/M_AlphaCombine.M_AlphaCombine'")));


	ColorBlendRenderTarget = Cast<UTextureRenderTarget2D>(StaticLoadObject(UTextureRenderTarget2D::StaticClass(), NULL,
		TEXT("TextureRenderTarget2D'/VolumetricCloudsPainter/Textures/RT_ColorBlend.RT_ColorBlend'")));
	AlphaBlendRenderTarget = Cast<UTextureRenderTarget2D>(StaticLoadObject(UTextureRenderTarget2D::StaticClass(), NULL,
		TEXT("TextureRenderTarget2D'/VolumetricCloudsPainter/Textures/RT_AlphaBlend.RT_AlphaBlend'")));

	RenderTarget = Cast<UTextureRenderTarget2D>(StaticLoadObject(UTextureRenderTarget2D::StaticClass(), NULL,
		TEXT("TextureRenderTarget2D'/VolumetricCloudsPainter/Textures/RT_FinalRenderTarget.RT_FinalRenderTarget'")));
}

FVolumetricCloudsPainterEdMode::~FVolumetricCloudsPainterEdMode()
{

}

void FVolumetricCloudsPainterEdMode::Enter()
{
	FEdMode::Enter();

	GetCloudsActor();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FVolumetricCloudsPainterEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

/** Find volumetric clouds actor in scene.*/
bool FVolumetricCloudsPainterEdMode::GetCloudsActor()
{
	bool bCloudsFound = false;

	for (TActorIterator<AStaticMeshActor> StaticMeshItr(GetWorld()); StaticMeshItr; ++StaticMeshItr)
	{
		AStaticMeshActor* SelectedCloudsActor = Cast<AStaticMeshActor>(*StaticMeshItr);

		if (SelectedCloudsActor != nullptr)
		{
			FName ActorClassName = SelectedCloudsActor->GetClass()->GetFName();

			if (ActorClassName == "VolumetricClouds_C")
			{
				CloudsActor = SelectedCloudsActor;
				CloudsMaterial = Cast<UMaterialInstanceConstant>(CloudsActor->GetStaticMeshComponent()->GetMaterial(0));

				UTexture* TempTexturePointer;
				bool bTextureFound = CloudsMaterial->GetTextureParameterValue(FMaterialParameterInfo("WeatherMap"), TempTexturePointer);

				if (bTextureFound)
				{
					FinalTexture = Cast<UTexture2D>(TempTexturePointer);
					LoadTexture();
				}

				bCloudsFound = true;
				break;
			}
		}
	}

	//If actor didn't found in a selection. Clear actor from selection even if it's was selected.
	if (!bCloudsFound)
	{
		ReleaseCoudsActor();
	}

	return bCloudsFound;
}

void FVolumetricCloudsPainterEdMode::Exit()
{
	ReleaseCoudsActor();

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

/** Release clouds actor. */
void FVolumetricCloudsPainterEdMode::ReleaseCoudsActor()
{
	SetPaintState(false);
	CloudsActor = nullptr;
	CloudsMaterial = nullptr;
	FinalTexture = nullptr;
}

bool FVolumetricCloudsPainterEdMode::UsesToolkits() const
{
	return true;
}

bool FVolumetricCloudsPainterEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	//Paint exit event.
	if (Key == EKeys::Escape && Event == EInputEvent::IE_Pressed)
	{
		SetPaintState(false);
	}

	//If actor is selected.
	if (CloudsActor != nullptr)
	{
		//In paint mode keyboard events.
		if (IsPainiting())
		{
			//Left mouse button.
			if (Key == EKeys::LeftMouseButton)
			{
				if (Event == EInputEvent::IE_Pressed)
				{
					bPressedLMB = true;
					return true;
				}

				if (Event == EInputEvent::IE_Released)
				{
					bPressedLMB = false;
					PreviousMousePosition = FVector2D(-10000.0, -10000.0);
					return false;
				}
			}

			//Paint mode toggle event.
			if (Key == EKeys::LeftControl)
			{
				if (Event == EInputEvent::IE_Pressed)
				{
					SetPaintMode(false);
				}

				if (Event == EInputEvent::IE_Released)
				{
					SetPaintMode(true);
				}
			}
		}

		//Paint toggle event.
		if (Key == EKeys::P && Event == EInputEvent::IE_Pressed)
		{
			SetPaintState(!bPainiting);
		}
	}



	return false;
}


void FVolumetricCloudsPainterEdMode::ActorSelectionChangeNotify()
{

}

void FVolumetricCloudsPainterEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	//Draw editor helper only if volumetric clouds class exist and editor mode in enabled painiting.
	if (CloudsActor != nullptr && CloudsMaterial != nullptr && IsPainiting())
	{
		if (GEditor != nullptr && GEditor->GetActiveViewport() != nullptr && GEditor->GetActiveViewport()->GetClient() != nullptr)
		{
			//Calculate world position for a brush.
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

			FVector ViewPosition = ViewportClient->GetViewLocation();
			FVector ViewDirection = ViewportClient->GetCursorWorldLocationFromMousePos().GetDirection();

			//Get actor material parameters.
			float WeatherMapSize = 1000.0;
			CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("WeatherMapSize"), WeatherMapSize);

			FVector CloudsPosition = CloudsActor->GetActorLocation();
			float GroundCloudsHeight = CloudsPosition.Z;
			CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("GroundCloudsHeight"), GroundCloudsHeight);
			float CloudsHeight = CloudsActor->GetActorScale3D().Z * 100.0f;
			CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("CloudsHeight"), CloudsHeight);

			//Find intersection with clouds lover plane.
			float RayLength = FVector::DotProduct(CloudsPosition - ViewPosition, FVector(0, 0, 1)) * (1.0f / FVector::DotProduct(ViewDirection, FVector(0, 0, 1)));

			//Brush world position.
			WorldBrushPos = ViewPosition + ViewDirection * RayLength;

			//Calculate brush radius in a world space.
			float WorldBrushRadius = (BrushRadius / 2.0f) * 1000000.0f * WeatherMapSize;

			//Some render helpers parameters. 
			//TODO: MOVE IT OUTSIE OF THE FUNCTION!
			float RenderHelpersThickness = FMath::Lerp(10.0f, 1000.0f, RayLength / GroundCloudsHeight);
			FLinearColor RenderHelpersColor = FLinearColor(0.0f, 1.0f, 1.0f, 0.25f);

			//Draw brush radius helper.
			DrawCircle(PDI, WorldBrushPos, FVector(1, 0, 0), FVector(0, 1, 0), RenderHelpersColor, WorldBrushRadius, 256, ESceneDepthPriorityGroup::SDPG_Foreground, RenderHelpersThickness);

			//Get normal for a brush position.
			FVector Normal = FVector(0.0f, 0.0f, 1.0f);
			//FVector Normal = (WorldBrushPos - ECenter);
			//Normal.Normalize();

			//Draw brush position normal.
			PDI->DrawLine(WorldBrushPos, WorldBrushPos - Normal * RenderHelpersThickness * 20.0f, RenderHelpersColor, ESceneDepthPriorityGroup::SDPG_Foreground, RenderHelpersThickness);
		}

	}

}

/** Draw a screen HUD information. */
void FVolumetricCloudsPainterEdMode::DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	//Draw this information only if painting is enabled.
	if (IsPainiting())
	{
		FIntPoint ViewportSize = Viewport->GetSizeXY();
		FIntPoint BaseMousePos;
		Viewport->GetMousePos(BaseMousePos);

		FVector2D MousePos = BaseMousePos;
		MousePos = MousePos / Canvas->GetDPIScale();

		FString PaintModeTooltip = FString("ADD");

		if (!bAdditivePaint)
		{
			PaintModeTooltip = FString("SUB");
		}

		Canvas->DrawShadowedString(MousePos.X + 10.0f, MousePos.Y + 10.0f, *PaintModeTooltip, GEngine->GetSmallFont(), FLinearColor(1, 1, 1, 1));
	}
}

/** Load texture to a render target and setup base parameters. */
void FVolumetricCloudsPainterEdMode::LoadTexture()
{
	FinalTextureSize = FVector2D(FinalTexture->GetSizeX(), FinalTexture->GetSizeY());

	RenderTarget->ResizeTarget(FinalTextureSize.X, FinalTextureSize.Y);
	ColorBlendRenderTarget->ResizeTarget(FinalTextureSize.X, FinalTextureSize.Y);
	AlphaBlendRenderTarget->ResizeTarget(FinalTextureSize.X, FinalTextureSize.Y);

	ColorBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);

	AlphaBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);

	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("RedChannelEnabled"), (float)bRedChannelEnabled);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("GreenChannelEnabled"), (float)bGreenChannelEnabled);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BlueChannelEnabled"), (float)bBlueChannelEnabled);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AlphaChannelEnabled"), (float)bAlphaChannelEnabled);

	//Base draw parameters.
	UCanvas* DrawCanvas;
	FVector2D DrawSize = FVector2D(1.0f, 1.0f);

	AlphaBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("TextureToRead"), FinalTexture);
	ColorBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("TextureToRead"), FinalTexture);

	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 1);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 1);

	//Base draw parameters.
	FDrawToRenderTargetContext DrawContext;

	//Render to color blend render target.
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ColorBlendRenderTarget, DrawCanvas, DrawSize, DrawContext);
	DrawCanvas->K2_DrawMaterial(ColorBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), DrawContext);

	//Base draw parameters.
	FDrawToRenderTargetContext AlphaBlendRenderTargetDrawContext;

	//Render to alpha blend render target.
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTarget, DrawCanvas, DrawSize, AlphaBlendRenderTargetDrawContext);
	DrawCanvas->K2_DrawMaterial(AlphaBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTargetDrawContext);

	//Update Render target.

	//Base draw parameters.
	FDrawToRenderTargetContext RenderTargetDrawContext;

	//Blend colors to one render target.
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, DrawCanvas, DrawSize, RenderTargetDrawContext);
	DrawCanvas->K2_DrawMaterial(AlphCombineMaterial, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), RenderTargetDrawContext);

	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 0);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 0);
}


/** Draw to a render target. */
void FVolumetricCloudsPainterEdMode::DrawToRenderTaget()
{
	if (CloudsActor != nullptr && CloudsMaterial != nullptr && RenderTarget != nullptr && FinalTexture != nullptr)
	{
		if (ColorBlendRenderTarget != nullptr && AlphaBlendRenderTarget != nullptr && ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
		{
			//Calculate brush screen position based on a world position of the brush.
			FVector2D ScreenPosition;
			ScreenPosition.X = WorldBrushPos.X;
			ScreenPosition.Y = WorldBrushPos.Y;

			//Get parameters from a material.
			float WeatherMapSize = 0.0;
			CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("WeatherMapSize"), WeatherMapSize);
			float RepeatSize = WeatherMapSize * 1000000.0f;


			ScreenPosition = (ScreenPosition + RepeatSize / 2.0f) / (RepeatSize);

			int BrushPosIndexX = (int)(ScreenPosition.X);
			int BrushPosIndexY = (int)(ScreenPosition.Y);

			ScreenPosition = ScreenPosition - FVector2D(BrushPosIndexX, BrushPosIndexY);

			ColorBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushPosition"), FVector(ScreenPosition.X, ScreenPosition.Y, 0));
			AlphaBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushPosition"), FVector(ScreenPosition.X, ScreenPosition.Y, 0));

			//Update brush channel values based on a paint state.
			SetPreRenderBrushParameters();

			//Base draw parameters.
			UCanvas* DrawCanvas;
			FVector2D DrawSize = FVector2D(1.0f, 1.0f);

			//Base draw parameters.
			FDrawToRenderTargetContext DrawContext;

			//Render to color blend render target.
			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ColorBlendRenderTarget, DrawCanvas, DrawSize, DrawContext);
			DrawCanvas->K2_DrawMaterial(ColorBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), DrawContext);

			//Base draw parameters.
			FDrawToRenderTargetContext AlphaBlendRenderTargetDrawContext;

			//Render to alpha blend render target.
			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTarget, DrawCanvas, DrawSize, AlphaBlendRenderTargetDrawContext);
			DrawCanvas->K2_DrawMaterial(AlphaBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTargetDrawContext);

			//Update Render target.

			//Base draw parameters.
			FDrawToRenderTargetContext RenderTargetDrawContext;

			//Blend colors to one render target.
			UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, DrawCanvas, DrawSize, RenderTargetDrawContext);
			DrawCanvas->K2_DrawMaterial(AlphCombineMaterial, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), RenderTargetDrawContext);
		}
	}

}

/** Update brush color parameters based on paint state when rendering to a texture. */
void FVolumetricCloudsPainterEdMode::SetPreRenderBrushParameters()
{
	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		if (!bAdditivePaint)
		{
			ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), -1);
			AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), -1);
		}
		else
		{
			ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), 1);
			AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), 1);
		}

	}
}

/** Update brush radius
* @param NewRadius - new brush radius.
*/
void FVolumetricCloudsPainterEdMode::SetBrushRadius(float NewRadius)
{
	BrushRadius = NewRadius;

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);
	}
}

/** Update brush falloff
* @param NewFalloff - new brush falloff.
*/
void FVolumetricCloudsPainterEdMode::SetBrushFalloff(float NewFalloff)
{
	BrushFalloff = NewFalloff;

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
	}
}


/** Update brush opacity
* @param NewOpacity - new brush opacity.
*/
void FVolumetricCloudsPainterEdMode::SetBrushOpacity(float NewOpacity)
{
	BrushOpacity = NewOpacity;

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1f);
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1f);
	}
}

/** Update brush color.
* @param NewValue - new brush channel value.
* @param Channel - brush channel.
*/
void FVolumetricCloudsPainterEdMode::SetBrushChannelValue(float NewValue, FName Channel)
{
	if (Channel == "RedChannel")
	{
		BrushColor.R = NewValue;
	}
	else if (Channel == "GreenChannel")
	{
		BrushColor.G = NewValue;
	}
	else if (Channel == "BlueChannel")
	{
		BrushColor.B = NewValue;
	}
	else if (Channel == "AlphaChannel")
	{
		BrushColor.A = NewValue;
	}

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
		AlphaBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
	}
}
/** Recieve brush color value by channel.
* @param Channel - brush color channel.
*/
float FVolumetricCloudsPainterEdMode::GetBrushChannelValue(FName Channel)
{
	float Value = 0.0f;

	if (Channel == "RedChannel")
	{
		Value = BrushColor.R;
	}
	else if (Channel == "GreenChannel")
	{
		Value = BrushColor.G;
	}
	else if (Channel == "BlueChannel")
	{
		Value = BrushColor.B;
	}
	else if (Channel == "AlphaChannel")
	{
		Value = BrushColor.A;
	}

	return Value;
}

/** Returns is channel currently enabled for painting.
* @param Channel - color channel.
*/
bool FVolumetricCloudsPainterEdMode::IsChannelEnabled(FName Channel)
{
	bool bEnabled = true;

	if (Channel == "RedChannel")
	{
		bEnabled = bRedChannelEnabled;
	}
	else if (Channel == "GreenChannel")
	{
		bEnabled = bGreenChannelEnabled;
	}
	else if (Channel == "BlueChannel")
	{
		bEnabled = bBlueChannelEnabled;
	}
	else if (Channel == "AlphaChannel")
	{
		bEnabled = bAlphaChannelEnabled;
	}

	return bEnabled;
}

/** Sets is channel state to a new one.
* @param NewValue - new channel value.
* @param Channel - color channel.
*/
void FVolumetricCloudsPainterEdMode::SetChannelState(bool NewValue, FName Channel)
{
	if (Channel == "RedChannel")
	{
		bRedChannelEnabled = NewValue;

		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("RedChannelEnabled"), (float)NewValue);

	}
	else if (Channel == "GreenChannel")
	{
		bGreenChannelEnabled = NewValue;
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("GreenChannelEnabled"), (float)NewValue);
	}
	else if (Channel == "BlueChannel")
	{
		bBlueChannelEnabled = NewValue;
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BlueChannelEnabled"), (float)NewValue);
	}
	else if (Channel == "AlphaChannel")
	{
		bAlphaChannelEnabled = NewValue;
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AlphaChannelEnabled"), (float)NewValue);
	}

	if (RenderTarget != nullptr)
	{
		ColorBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("RenderTarget"), RenderTarget);
		AlphaBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("RenderTarget"), RenderTarget);
	}
}

/** Change painting state.
* @param NewState - new paint state.
*/
void FVolumetricCloudsPainterEdMode::SetPaintState(bool newState)
{
	bPainiting = newState;

	if (!bPainiting)
	{
		if (CloudsMaterial != nullptr && FinalTexture != nullptr)
		{
			CloudsMaterial->SetTextureParameterValueEditorOnly(FName("WeatherMap"), FinalTexture);
			UKismetRenderingLibrary::ConvertRenderTargetToTexture2DEditorOnly(GetWorld(), RenderTarget, FinalTexture);
		}
	}
	else
	{
		LoadTexture();

		if (RenderTarget != nullptr && FinalTexture != nullptr)
		{
			//Temporally switch texture to a render target.
			CloudsMaterial->SetTextureParameterValueEditorOnly(FName("WeatherMap"), RenderTarget);
		}
	}

	if (CloudsMaterial != nullptr)
	{
		CloudsMaterial->PostLoad();
	}

};

/** Tick function for every frame. */
void FVolumetricCloudsPainterEdMode::Tick(FEditorViewportClient * ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);

	//Draw this information only if painting is enabled.
	if (IsPainiting())
	{
		FViewport* Viewport = ViewportClient->Viewport;
		FIntPoint ViewportSize = Viewport->GetSizeXY();
		FIntPoint BaseMousePos;
		Viewport->GetMousePos(BaseMousePos);

		if (bPressedLMB && PreviousMousePosition != FVector2D(BaseMousePos))
		{
			DrawToRenderTaget();
			PreviousMousePosition = FVector2D(BaseMousePos);
		}
	}
}