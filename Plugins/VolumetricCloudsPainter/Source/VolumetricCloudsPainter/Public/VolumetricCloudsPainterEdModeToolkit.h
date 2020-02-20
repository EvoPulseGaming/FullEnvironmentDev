// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"





class FVolumetricCloudsPainterEdModeToolkit : public FModeToolkit
{
public:

	FVolumetricCloudsPainterEdModeToolkit();

	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:

	TSharedPtr<SWidget> ToolkitWidget;



private:
	/** Path to the object */
	TAttribute<FString> ObjectPath;

	/** The widget used to edit the object 'property' */
	TSharedPtr<SPropertyEditorAsset> PropertyEditorAsset;

	/** Handle to a property we modify (if any)*/
	TSharedPtr<IPropertyHandle> PropertyHandle;
	/** Delegate to call to determine whether the asset should be set */
	FOnShouldSetAsset OnShouldSetAsset;


	FCheckBoxStyle PaintCheckBoxStyle;
	FCheckBoxStyle PaintTypeCheckBoxStyle;


	/** Is editor mode have a clouds actor. */
	bool IsActorSelected() const;

	/** Event that called when painter checkbox state changed.
	* @param newState - new checkbox state.
	*/
	void OnPainterCheckBoxStateChanged(ECheckBoxState newState);
	/** Is painter checkbox is checked. */
	ECheckBoxState IsPainterCheckBoxChecked() const;
	/** Painter check box state. */
	//ECheckBoxState PainterCheckBoxState = ECheckBoxState::Unchecked;
	/** Painter button check box text. */
	FText PainterCheckBoxText() const;

	/** Event that called when painter paint mode checkbox state changed.
	* @param newState - new checkbox state.
	*/
	void OnPaintModeCheckBoxStateChanged(ECheckBoxState newState);
	/** Is paint mode checkbox is checked. */
	ECheckBoxState IsPaintModeCheckBoxChecked() const;
	/** Erase check box state. */
	//ECheckBoxState PaintModeCheckBoxState = ECheckBoxState::Checked;

	/** Event that called when painter erase mode checkbox state changed.
	* @param newState - new checkbox state.
	*/
	void OnEraseModeCheckBoxStateChanged(ECheckBoxState newState);
	/** Is erase mode checkbox is checked. */
	ECheckBoxState IsEraseModeCheckBoxChecked() const;
	/** Erase check box state. */
	//ECheckBoxState EraseModeCheckBoxState = ECheckBoxState::Unchecked;

	/** Event that called when channel checkbox state changed.	*/
	void OnChannelCheckBoxStateChanged(ECheckBoxState newState, FName ChannelName);
	ECheckBoxState IsChannelBoxChecked(FName ChannelName) const;
	FText ChannelCheckBoxText(FName ChannelName) const;

};

