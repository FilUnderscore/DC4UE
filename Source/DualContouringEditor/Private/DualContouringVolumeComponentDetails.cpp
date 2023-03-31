#include "DualContouringVolumeComponentDetails.h"
#include "DualContouringVolumeComponent.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "DualContouringVolumeComponentDetails"

TSharedRef<IDetailCustomization> FDualContouringVolumeComponentDetails::MakeInstance()
{
	return MakeShareable(new FDualContouringVolumeComponentDetails);
}

void FDualContouringVolumeComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Dual Contouring Volume");
	
	const FText RegenerateText = LOCTEXT("Regenerate", "Regenerate");

	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	Category.AddCustomRow(RegenerateText, false)
	.NameContent()
	[
		SNullWidget::NullWidget
	]
	.ValueContent()
	.VAlign(VAlign_Center)
	.MaxDesiredWidth(250)
	[
		SNew(SButton)
		.VAlign(VAlign_Center)
		.ToolTipText(LOCTEXT("RegenerateTooltip", "Regenerate this Dual Contouring Volume and its associated DualContouringVolumeMeshComponent."))
		.OnClicked(this, &FDualContouringVolumeComponentDetails::ClickedOnRegenerate)
		.IsEnabled(&FDualContouringVolumeComponentDetails::RegenerateEnabled)
		.Content()
		[
			SNew(STextBlock)
			.Text(RegenerateText)
		]
	];
}

TArray<UDualContouringVolumeComponent*> FDualContouringVolumeComponentDetails::GetDCVolumeComponents()
{
	TArray<UDualContouringVolumeComponent*> DCVolumeComponents;
	DCVolumeComponents.Reserve(SelectedObjectsList.Num());

	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		UDualContouringVolumeComponent* DCVolumeComp = Cast<UDualContouringVolumeComponent>(Object.Get());

		if (DCVolumeComp != nullptr && !DCVolumeComp->IsTemplate())
			DCVolumeComponents.Add(DCVolumeComp);
	}

	return DCVolumeComponents;
}

bool FDualContouringVolumeComponentDetails::RegenerateEnabled()
{
	return GetDCVolumeComponents().Num() > 0;
}

FReply FDualContouringVolumeComponentDetails::ClickedOnRegenerate()
{
	TArray<UDualContouringVolumeComponent*> DCVolumeComponents = GetDCVolumeComponents();

	if (DCVolumeComponents.Num() > 0)
	{
		for (UDualContouringVolumeComponent*& DCVolumeComp : DCVolumeComponents)
		{
			DCVolumeComp->Regenerate();
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE