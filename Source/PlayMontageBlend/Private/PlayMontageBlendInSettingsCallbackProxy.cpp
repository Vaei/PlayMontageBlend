// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayMontageBlendInSettingsCallbackProxy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayMontageBlendInSettingsCallbackProxy)

////////////////////////////////////////////////////////////////////////
// UPlayMontageTransposedCallbackProxy

 UPlayMontageBlendInSettingsCallbackProxy::UPlayMontageBlendInSettingsCallbackProxy(const FObjectInitializer& ObjectInitializer)
 	: Super(ObjectInitializer)
 	, MontageInstanceID(INDEX_NONE)
 	, bInterruptedCalledBeforeBlendingOut(false)
 {
 }

 UPlayMontageBlendInSettingsCallbackProxy* UPlayMontageBlendInSettingsCallbackProxy::CreateProxyObjectForPlayMontage(
 	class USkeletalMeshComponent* InSkeletalMeshComponent,
 	class UAnimMontage* MontageToPlay,
 	FMontageBlendSettings BlendIn,
 	float PlayRate,
 	float StartingPosition,
 	FName StartingSection)
 {
 	UPlayMontageBlendInSettingsCallbackProxy* Proxy = NewObject<UPlayMontageBlendInSettingsCallbackProxy>();
 	Proxy->SetFlags(RF_StrongRefOnFrame);
 	Proxy->PlayMontage(InSkeletalMeshComponent, MontageToPlay, BlendIn, PlayRate, StartingPosition, StartingSection);
 	return Proxy;
 }


 void UPlayMontageBlendInSettingsCallbackProxy::PlayMontage(
 	const class USkeletalMeshComponent* InSkeletalMeshComponent, 
	class UAnimMontage* MontageToPlay,
	FMontageBlendSettings BlendIn,
	float PlayRate,
	float StartingPosition,
	FName StartingSection)
 {
 	bool bPlayedSuccessfully = false;
 	if (InSkeletalMeshComponent)
 	{
 		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
 		{
 			const float MontageLength = AnimInstance->Montage_PlayWithBlendSettings(MontageToPlay, BlendIn, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition);
 			bPlayedSuccessfully = (MontageLength > 0.f);

 			if (bPlayedSuccessfully)
 			{
 				AnimInstancePtr = AnimInstance;
 				if (const FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
 				{
 					MontageInstanceID = MontageInstance->GetInstanceID();
 				}

 				if (StartingSection != NAME_None)
 				{
 					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
 				}

 				BlendingOutDelegate.BindUObject(this, &UPlayMontageBlendInSettingsCallbackProxy::OnMontageBlendingOut);
 				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

 				MontageEndedDelegate.BindUObject(this, &UPlayMontageBlendInSettingsCallbackProxy::OnMontageEnded);
 				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

 				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UPlayMontageBlendInSettingsCallbackProxy::OnNotifyBeginReceived);
 				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UPlayMontageBlendInSettingsCallbackProxy::OnNotifyEndReceived);
 			}
 		}
 	}

 	if (!bPlayedSuccessfully)
 	{
 		OnInterrupted.Broadcast(NAME_None);
 	}
 }

 bool UPlayMontageBlendInSettingsCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
 {
 	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
 }


 void UPlayMontageBlendInSettingsCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
 {
 	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
 	{
 		OnNotifyBegin.Broadcast(NotifyName);
 	}
 }


 void UPlayMontageBlendInSettingsCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
 {
 	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
 	{
 		OnNotifyEnd.Broadcast(NotifyName);
 	}
 }


 void UPlayMontageBlendInSettingsCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
 {
 	if (bInterrupted)
 	{
 		OnInterrupted.Broadcast(NAME_None);
 		bInterruptedCalledBeforeBlendingOut = true;
 	}
 	else
 	{
 		OnBlendOut.Broadcast(NAME_None);
 	}
 }

 void UPlayMontageBlendInSettingsCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
 {
 	if (!bInterrupted)
 	{
 		OnCompleted.Broadcast(NAME_None);
 	}
 	else if (!bInterruptedCalledBeforeBlendingOut)
 	{
 		OnInterrupted.Broadcast(NAME_None);
 	}

 	UnbindDelegates();
 }

 void UPlayMontageBlendInSettingsCallbackProxy::UnbindDelegates()
 {
 	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
 	{
 		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UPlayMontageBlendInSettingsCallbackProxy::OnNotifyBeginReceived);
 		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UPlayMontageBlendInSettingsCallbackProxy::OnNotifyEndReceived);
 	}
 }

 void UPlayMontageBlendInSettingsCallbackProxy::BeginDestroy()
 {
 	UnbindDelegates();

 	Super::BeginDestroy();
 }
