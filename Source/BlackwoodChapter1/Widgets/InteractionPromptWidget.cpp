#include "InteractionPromptWidget.h"
#include "Components/TextBlock.h"

void UInteractionPromptWidget::SetPromptText(const FText& NewText)
{
	if (PromptText)
	{
		PromptText->SetText(NewText);
	}
}