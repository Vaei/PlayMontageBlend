# PlayMontageBlend
This adds blend in and out options to the gameplay ability Play Montage node.

This allows for scaling the blend times by the play rate so you get the same result each time, just at different speed.

Inertialization is NOT SUPPORTED for blend out.

You MUST also [use this plugin](https://github.com/Vaei/AbilitySystemFix) if you want GAS montages to be capable of using any blend out time.

Here is an example function you can use to scale blend in and out time by playrate:

```cpp
UFUNCTION(BlueprintPure, Category="Ability")
static void GetBlendSettings(const UAnimMontage* Montage, float RateScale, FMontageBlendSettings& BlendIn, FAlphaBlend& BlendOut)
{
	if (!Montage || RateScale <= 0.f) {	return;	}
	
	BlendIn = { Montage->GetBlendInArgs() };
	BlendIn.Blend.BlendTime /= RateScale;
	BlendIn.BlendProfile = Montage->BlendProfileIn;
	BlendIn.BlendMode = Montage->BlendModeIn;

	BlendOut = { Montage->BlendOut };
	BlendOut.SetBlendTime(BlendOut.GetBlendTime() / RateScale);
}
  ```
