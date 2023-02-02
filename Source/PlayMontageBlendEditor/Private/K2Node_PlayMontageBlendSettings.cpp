// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_PlayMontageBlendSettings.h"

#include "PlayMontageBlendSettingsCallbackProxy.h"
#include "Containers/UnrealString.h"
#include "EdGraph/EdGraphPin.h"
#include "HAL/Platform.h"
#include "Internationalization/Internationalization.h"
#include "Misc/AssertionMacros.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectPtr.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_PlayMontageBlendSettings::UK2Node_PlayMontageBlendSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UPlayMontageBlendSettingsCallbackProxy, CreateProxyObjectForPlayMontage);
	ProxyFactoryClass = UPlayMontageBlendSettingsCallbackProxy::StaticClass();
	ProxyClass = UPlayMontageBlendSettingsCallbackProxy::StaticClass();
}

FText UK2Node_PlayMontageBlendSettings::GetTooltipText() const
{
	return LOCTEXT("K2Node_PlayMontage_Tooltip", "Plays a Montage on a SkeletalMeshComponent with custom blend in setting");
}

FText UK2Node_PlayMontageBlendSettings::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PlayMontage", "Play Montage (With Blend)");
}

FText UK2Node_PlayMontageBlendSettings::GetMenuCategory() const
{
	return LOCTEXT("PlayMontageCategory", "Animation|Montage");
}

void UK2Node_PlayMontageBlendSettings::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		const FText ToolTipText = LOCTEXT("K2Node_PlayMontageBlendSettings_OnNotifyBegin_Tooltip", "Event called when using a PlayMontageNotify or PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		const FText ToolTipText = LOCTEXT("K2Node_PlayMontageBlendSettings_OnNotifyEnd_Tooltip", "Event called when using a PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}

#undef LOCTEXT_NAMESPACE
