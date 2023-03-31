#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FDualContouringEditorModule : public IModuleInterface
{
private:
	TArray<FName> VisualizersToUnregisterOnShutdown;
	TArray<FName> PropertyEditorsToUnregisterOnShutdown;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};