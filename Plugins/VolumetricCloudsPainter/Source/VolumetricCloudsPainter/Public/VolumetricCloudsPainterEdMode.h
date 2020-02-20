// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

#include "Materials/MaterialInstanceConstant.h"
#include "Engine/StaticMeshActor.h"

class FVolumetricCloudsPainterEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_VolumetricCloudsPainterEdModeId;
public:
	FVolumetricCloudsPainterEdMode();
	virtual ~FVolumetricCloudsPainterEdMode();

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	//virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void ActorSelectionChangeNotify() override;

	/** Find volumetric clouds actor in scene.*/
	bool GetCloudsActor();

	/** Release clouds actor. */
	void ReleaseCoudsActor();

	/** Draw a screen HUD information. */
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	bool UsesToolkits() const override;

	/** Previous mouse position for delta calculation. */
	FVector2D PreviousMousePosition = FVector2D(-10000.0, -10000.0);

	/** Is left mouse buttom pressed. */
	bool bPressedLMB = false;

	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;

	// End of FEdMode interface

	/** Selected clouds actor. */
	AStaticMeshActor* CloudsActor = nullptr;

	/** Selected clouds material. */
	UMaterialInstanceConstant* CloudsMaterial = nullptr;

	/** World position of the brush. */
	FVector WorldBrushPos;

	/** Painting state. */
	bool bPainiting = false;
	/** Is painiting enabled. */
	bool IsPainiting() { return bPainiting; };
	/** Change painting state.
	* @param NewState - new paint state.
	*/
	void SetPaintState(bool newState);


	/** Is painting mode set to add to a color. */
	bool bAdditivePaint = true;
	/** Is painting mode set is add or erase. */
	bool IsAdditivePaint() { return bAdditivePaint; };
	/** Change painting mode.
	* @param NewState - new paint mode.
	*/
	void SetPaintMode(bool newState) { bAdditivePaint = newState; };

	/** Render target texture. */
	UTextureRenderTarget2D* RenderTarget = nullptr;

	/** Clouds texture to save from render target. */
	UTexture2D* FinalTexture = nullptr;

	/** Final texture size. */
	FVector2D FinalTextureSize;

	/** Is base texture loaded. */
	bool bTextureNeedToLoad = true;
	/** Load texture to a render target and setup base parameters. */
	void LoadTexture();

	/** Painter color blend material instance. */
	UMaterialInstanceConstant* ColorBlendMaterialInstance = nullptr;
	/** Painter alpha blend material instance. */
	UMaterialInstanceConstant* AlphaBlendMaterialInstance = nullptr;
	/** Painter material for combining alpha with a RGB values. */
	UMaterial* AlphCombineMaterial = nullptr;
	/** Color blend render target. */
	UTextureRenderTarget2D* ColorBlendRenderTarget = nullptr;
	/** Alpha blend render target. */
	UTextureRenderTarget2D* AlphaBlendRenderTarget = nullptr;

	/** Brush radius in a UV (0-1) coordinates divided by 2. */
	float BrushRadius = 0.1f;
	/** Update brush radius
	* @param NewRadius - new brush radius.
	*/
	void SetBrushRadius(float NewRadius);
	/** Recieve brush opacity parameter. */
	float GetBrushRadius() { return BrushRadius; };

	/** Brush falloff parameter. */
	float BrushFalloff = 1.0f;
	/** Update brush falloff
	* @param NewFalloff - new brush falloff.
	*/
	void SetBrushFalloff(float NewFalloff);
	/** Recieve brush falloff parameter. */
	float GetBrushFalloff() { return BrushFalloff; };

	/** Brush opacity parameter. */
	float BrushOpacity = 0.25f;
	/** Update brush opacity
	* @param NewOpacity - new brush opacity.
	*/
	void SetBrushOpacity(float NewOpacity);
	/** Recieve brush opacity parameter. */
	float GetBrushOpacity() { return BrushOpacity; };

	/** Draw to a render target. */
	void DrawToRenderTaget();

	/** Update brush color parameters based on paint state when rendering to a texture. */
	void SetPreRenderBrushParameters();

	/** Brush color. */
	FLinearColor BrushColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	/** Update brush color.
	* @param NewValue - new brush channel value.
	* @param Channel - brush channel.
	*/
	void SetBrushChannelValue(float NewValue, FName Channel);
	/** Recieve brush color value by channel.
	* @param Channel - brush color channel.
	*/
	float GetBrushChannelValue(FName Channel);

	/** Is channel painting enabled. */
	bool bRedChannelEnabled = true;
	bool bGreenChannelEnabled = true;
	bool bBlueChannelEnabled = false;
	bool bAlphaChannelEnabled = false;

	/** Returns is channel currently enabled for painting.
	* @param Channel - color channel.
	*/
	bool IsChannelEnabled(FName Channel);

	/** Sets is channel state to a new one.
	* @param NewValue - new channel value.
	* @param Channel - color channel.
	*/
	void SetChannelState(bool NewValue, FName Channel);

	/** Tick function for every frame. */
	virtual void Tick(FEditorViewportClient * ViewportClient, float DeltaTime) override;
};
