// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "VolumetricCloudsPainter.h"
#include "VolumetricCloudsPainterEdMode.h"

#define LOCTEXT_NAMESPACE "FVolumetricCloudsPainterModule"

void FVolumetricCloudsPainterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FVolumetricCloudsPainterEdMode>(FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId, LOCTEXT("VolumetricCloudsPainterEdModeName", "VolumetricCloudsPainterEdMode"), FSlateIcon(), true);
}

void FVolumetricCloudsPainterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVolumetricCloudsPainterModule, VolumetricCloudsPainter)