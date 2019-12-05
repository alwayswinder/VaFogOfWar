// Copyright 2019 Vladimir Alyamkin. All Rights Reserved.

#include "VaFogBlockingVolume.h"

#include "VaFogDefines.h"
#include "VaFogLayer.h"

#include "Components/BillboardComponent.h"
#include "Components/BrushComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"

AVaFogBlockingVolume::AVaFogBlockingVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));

	if (!IsRunningCommandlet() && (SpriteComponent != nullptr))
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TextRenderTexture;
			FConstructorStatics()
				: TextRenderTexture(TEXT("/Engine/EditorResources/S_VectorFieldVol"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		SpriteComponent->Sprite = ConstructorStatics.TextRenderTexture.Get();
		SpriteComponent->RelativeScale3D = FVector(1.f, 1.f, 1.f);
		SpriteComponent->SetupAttachment(GetBrushComponent());
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->bAbsoluteScale = true;
		SpriteComponent->bReceivesDecals = false;
	}
#endif

	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(0, 255, 0, 255);
	bColored = true;
}

void AVaFogBlockingVolume::PostLoad()
{
	Super::PostLoad();

	// @TODO Layers can be initialized AFTER
	//UpdateTargetLayer();
}

void AVaFogBlockingVolume::PostActorCreated()
{
	Super::PostActorCreated();

	// @TODO Layers can be initialized AFTER
	//UpdateTargetLayer();
}

void AVaFogBlockingVolume::OnConstruction(const FTransform& Transform)
{
	UpdateTargetLayer();
}

#if WITH_EDITOR
void AVaFogBlockingVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateTargetLayer();

	// @TODO Force volume brush to be square or use custom BrushBuilder

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AVaFogBlockingVolume::UpdateTargetLayer()
{
	UE_LOG(LogVaFog, Warning, TEXT("[%s] Volume [%s] Check layer we should update and apply self into"), *VA_FUNC_LINE, *GetName());

	if (Layer)
	{
		Layer->AddFogBlockingVolume(this);
		Layer->UpdateLayer(true);
	}
}
