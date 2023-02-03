// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayMontageBlendSettingsCallbackProxy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayMontageBlendSettingsCallbackProxy)

////////////////////////////////////////////////////////////////////////
// UPlayMontageTransposedCallbackProxy

 UPlayMontageBlendSettingsCallbackProxy::UPlayMontageBlendSettingsCallbackProxy(const FObjectInitializer& ObjectInitializer)
 	: Super(ObjectInitializer)
 	, MontageInstanceID(INDEX_NONE)
 	, bInterruptedCalledBeforeBlendingOut(false)
 {
 }

 UPlayMontageBlendSettingsCallbackProxy* UPlayMontageBlendSettingsCallbackProxy::CreateProxyObjectForPlayMontage(
 	class USkeletalMeshComponent* InSkeletalMeshComponent,
 	class UAnimMontage* MontageToPlay,
 	FMontageBlendSettings BlendIn,
 	FAlphaBlend BlendOut,
 	float PlayRate,
 	float StartingPosition,
 	FName StartingSection)
 {
 	UPlayMontageBlendSettingsCallbackProxy* Proxy = NewObject<UPlayMontageBlendSettingsCallbackProxy>();
 	Proxy->SetFlags(RF_StrongRefOnFrame);
 	Proxy->PlayMontage(InSkeletalMeshComponent, MontageToPlay, BlendIn, BlendOut, PlayRate, StartingPosition, StartingSection);
 	return Proxy;
 }


 void UPlayMontageBlendSettingsCallbackProxy::PlayMontage(
 	const class USkeletalMeshComponent* InSkeletalMeshComponent, 
	class UAnimMontage* MontageToPlay,
	FMontageBlendSettings BlendIn,
	FAlphaBlend BlendOut,
	float PlayRate,
	float StartingPosition,
	FName StartingSection)
 {
 	bool bPlayedSuccessfully = false;
 	if (InSkeletalMeshComponent)
 	{
 		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
 		{
 			PrevMontage = MontageToPlay;
 			PrevBlendOut = MontageToPlay->BlendOut;
 			MontageToPlay->BlendOut = BlendOut;
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

 				BlendingOutDelegate.BindUObject(this, &UPlayMontageBlendSettingsCallbackProxy::OnMontageBlendingOut);
 				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

 				MontageEndedDelegate.BindUObject(this, &UPlayMontageBlendSettingsCallbackProxy::OnMontageEnded);
 				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

 				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UPlayMontageBlendSettingsCallbackProxy::OnNotifyBeginReceived);
 				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UPlayMontageBlendSettingsCallbackProxy::OnNotifyEndReceived);
 			}
 		}
 	}

 	if (!bPlayedSuccessfully)
 	{
 		OnInterrupted.Broadcast(NAME_None);
 	}
 }

 bool UPlayMontageBlendSettingsCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
 {
 	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
 }


 void UPlayMontageBlendSettingsCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
 {
 	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
 	{
 		OnNotifyBegin.Broadcast(NotifyName);
 	}
 }


 void UPlayMontageBlendSettingsCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
 {
 	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
 	{
 		OnNotifyEnd.Broadcast(NotifyName);
 	}
 }


 void UPlayMontageBlendSettingsCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
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

 void UPlayMontageBlendSettingsCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
 {
 	if (!bInterrupted)
 	{
 		OnCompleted.Broadcast(NAME_None);
 	}
 	else if (!bInterruptedCalledBeforeBlendingOut)
 	{
 		OnInterrupted.Broadcast(NAME_None);
 	}

 	RestorePrevMontage();
 	
 	UnbindDelegates();
 }

 void UPlayMontageBlendSettingsCallbackProxy::UnbindDelegates()
 {
 	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
 	{
 		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UPlayMontageBlendSettingsCallbackProxy::OnNotifyBeginReceived);
 		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UPlayMontageBlendSettingsCallbackProxy::OnNotifyEndReceived);
 	}
 }

 void UPlayMontageBlendSettingsCallbackProxy::BeginDestroy()
 {
 	UnbindDelegates();

 	RestorePrevMontage();

 	Super::BeginDestroy();
 }
