#pragma once

#include "IDetailCustomization.h"

class UDualContouringVolumeComponent;

class FDualContouringVolumeComponentDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

	TArray<UDualContouringVolumeComponent*> GetDCVolumeComponents();

	bool RegenerateEnabled();
	FReply ClickedOnRegenerate();
};