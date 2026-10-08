#include "HeldItem.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/PickupItemData.h"

AHeldItem::AHeldItem()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Held items are purely visual.
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetSimulatePhysics(false);

	ItemData = nullptr;
}

void AHeldItem::SetItemData(UPickupItemData* NewItemData)
{
	ItemData = NewItemData;

	if (!MeshComponent || !ItemData)
	{
		return;
	}

	// Use the dedicated held mesh if one exists.
	// Otherwise fall back to the world mesh.
	UStaticMesh* MeshToUse = ItemData->HeldMesh;

	if (!MeshToUse)
	{
		MeshToUse = ItemData->WorldMesh;
	}

	MeshComponent->SetStaticMesh(MeshToUse);
}

void AHeldItem::AttachToCamera(AActor* OwnerActor)
{
	if (!OwnerActor)
	{
		return;
	}

	UCameraComponent* CameraComponent =
		OwnerActor->FindComponentByClass<UCameraComponent>();

	if (!CameraComponent)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("AHeldItem::AttachToCamera: OwnerActor '%s' has no CameraComponent."),
			*OwnerActor->GetName()
		);

		return;
	}

	AttachToComponent(
		CameraComponent,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);
}

void AHeldItem::DetachAndDrop()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Destroy();
}

void AHeldItem::BeginPlay()
{
	Super::BeginPlay();
}