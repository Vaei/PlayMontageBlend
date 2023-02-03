// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Animation/AnimInstance.h"
#include "PlayMontageBlendSettingsCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontageBlendPlayDelegate, FName, NotifyName);

UCLASS(MinimalAPI)
class UPlayMontageBlendSettingsCallbackProxy : public UObject
{
	GENERATED_UCLASS_BODY()

	// Called when Montage finished playing and wasn't interrupted
	UPROPERTY(BlueprintAssignable)
	FOnMontageBlendPlayDelegate OnCompleted;

	// Called when Montage starts blending out and is not interrupted
	UPROPERTY(BlueprintAssignable)
	FOnMontageBlendPlayDelegate OnBlendOut;

	// Called when Montage has been interrupted (or failed to play)
	UPROPERTY(BlueprintAssignable)
	FOnMontageBlendPlayDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FOnMontageBlendPlayDelegate OnNotifyBegin;

	UPROPERTY(BlueprintAssignable)
	FOnMontageBlendPlayDelegate OnNotifyEnd;

	UPROPERTY(BlueprintAssignable)
	FOnMontageBlendPlayDelegate OnTaskComplete;

	// Called to perform the query internally
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UPlayMontageBlendSettingsCallbackProxy* CreateProxyObjectForPlayMontage(
		class USkeletalMeshComponent* InSkeletalMeshComponent,
		class UAnimMontage* MontageToPlay,
		FMontageBlendSettings BlendIn,
		FAlphaBlend BlendOut,
		float PlayRate = 1.f,
		float StartingPosition = 0.f,
		FName StartingSection = NAME_None);

public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

	virtual ~UPlayMontageBlendSettingsCallbackProxy() override
	{
		RestorePrevMontage();
	}

protected:
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

private:
	TWeakObjectPtr<UAnimInstance> AnimInstancePtr;
	int32 MontageInstanceID;
	uint32 bInterruptedCalledBeforeBlendingOut : 1;

	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;
	void UnbindDelegates();

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	// Restore the previous montage settings, otherwise they don't recover until editor restart
	// Unfortunately none of the UAnimInstance Montage_Play functions take blend out settings so
	// this is a bit of a hack
	TWeakObjectPtr<UAnimMontage> PrevMontage;
	FAlphaBlend PrevBlendOut;
	FTimerHandle PrevMontageTimerHandle;
	void RestorePrevMontage()
	{
		if (PrevMontage.IsValid())
		{
			PrevMontage->BlendOut = PrevBlendOut;
			PrevMontage.Reset();
		}
	}

	void PlayMontage(
		const class USkeletalMeshComponent* InSkeletalMeshComponent,
		class UAnimMontage* MontageToPlay,
		FMontageBlendSettings BlendIn,
		FAlphaBlend BlendOut,
		float PlayRate = 1.f,
		float StartingPosition = 0.f,
		FName StartingSection = NAME_None);
};
