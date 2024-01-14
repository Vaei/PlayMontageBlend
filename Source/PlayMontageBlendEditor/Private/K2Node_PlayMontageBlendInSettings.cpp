// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_PlayMontageBlendInSettings.h"

#include "PlayMontageBlendInSettingsCallbackProxy.h"
#include "Containers/UnrealString.h"
#include "EdGraph/EdGraphPin.h"
#include "HAL/Platform.h"
#include "Internationalization/Internationalization.h"
#include "Misc/AssertionMacros.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_PlayMontageBlendInSettings)

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_PlayMontageBlendInSettings::UK2Node_PlayMontageBlendInSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UPlayMontageBlendInSettingsCallbackProxy, CreateProxyObjectForPlayMontage);
	ProxyFactoryClass = UPlayMontageBlendInSettingsCallbackProxy::StaticClass();
	ProxyClass = UPlayMontageBlendInSettingsCallbackProxy::StaticClass();
}

FText UK2Node_PlayMontageBlendInSettings::GetTooltipText() const
{
	return LOCTEXT("K2Node_PlayMontage_Tooltip", "Plays a Montage on a SkeletalMeshComponent with custom blend in setting");
}

FText UK2Node_PlayMontageBlendInSettings::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PlayMontage", "Play Montage (With Blend In)");
}

FText UK2Node_PlayMontageBlendInSettings::GetMenuCategory() const
{
	return LOCTEXT("PlayMontageCategory", "Animation|Montage");
}

void UK2Node_PlayMontageBlendInSettings::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		const FText ToolTipText = LOCTEXT("K2Node_PlayMontageBlendInSettings_OnNotifyBegin_Tooltip", "Event called when using a PlayMontageNotify or PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		const FText ToolTipText = LOCTEXT("K2Node_PlayMontageBlendInSettings_OnNotifyEnd_Tooltip", "Event called when using a PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}

#undef LOCTEXT_NAMESPACE
