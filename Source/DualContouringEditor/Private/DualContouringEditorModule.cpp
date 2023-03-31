#include "DualContouringEditorModule.h"
#include "DualContouringVolumeComponent.h"
#include "DualContouringVolumeComponentVisualizer.h"
#include "DualContouringVolumeComponentDetails.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

IMPLEMENT_MODULE(FDualContouringEditorModule, DualContouringEditor);

void FDualContouringEditorModule::StartupModule()
{
	VisualizersToUnregisterOnShutdown.Reset();
	PropertyEditorsToUnregisterOnShutdown.Reset();

	if (GUnrealEd)
	{
		TSharedPtr<FDualContouringVolumeComponentVisualizer> DCVolumeComponentVisualizer = MakeShared<FDualContouringVolumeComponentVisualizer>();
		GUnrealEd->RegisterComponentVisualizer(UDualContouringVolumeComponent::StaticClass()->GetFName(), DCVolumeComponentVisualizer);

		DCVolumeComponentVisualizer->OnRegister();
		VisualizersToUnregisterOnShutdown.Add(UDualContouringVolumeComponent::StaticClass()->GetFName());
	}

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(UDualContouringVolumeComponent::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FDualContouringVolumeComponentDetails::MakeInstance));
	PropertyEditorsToUnregisterOnShutdown.Add(UDualContouringVolumeComponent::StaticClass()->GetFName());
}

void FDualContouringEditorModule::ShutdownModule()
{
	if (GUnrealEd)
	{
		for (const FName& Name : VisualizersToUnregisterOnShutdown)
		{
			GUnrealEd->UnregisterComponentVisualizer(Name);
		}
	}

	FPropertyEditorModule* PropertyEditorModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyEditorModule)
	{
		for (FName ClassName : PropertyEditorsToUnregisterOnShutdown)
		{
			PropertyEditorModule->UnregisterCustomClassLayout(ClassName);
		}
	}
}