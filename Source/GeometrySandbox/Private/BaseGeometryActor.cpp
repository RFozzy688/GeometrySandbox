// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGeometryActor.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

// ����������� ��������� �����������
// ������ ������ �������� �� �������� ��������� ��������� �����������
// �. �. �������� ������ � ������ ������� cpp-�����
DEFINE_LOG_CATEGORY_STATIC(LogBaseGeometry, All, All)

// Sets default values
ABaseGeometryActor::ABaseGeometryActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	SetRootComponent(BaseMesh);
}

// Called when the game starts or when spawned
void ABaseGeometryActor::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation(); // ��������� ������ �������������

	//printTypes();
	//printStringTypes();
	//printTransform();

	SetColor(GeometryData.Color);

	// ������ �-��� ���������� ����� TimerManager
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseGeometryActor::OnTimerFired, GeometryData.TimerRate, true);
}

void ABaseGeometryActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogBaseGeometry, Error, TEXT("Actor is dead: %s"), *GetName());
}

// Called every frame 
void ABaseGeometryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMovement();
}

void ABaseGeometryActor::printTypes()
{
	//UE_LOG(LogTemp, Display, TEXT("Hello Unreal"));
	//UE_LOG(LogTemp, Warning, TEXT("Hello Unreal"));
	//UE_LOG(LogTemp, Error, TEXT("Hello Unreal"));

	// ������ ����� ����� GetName(). ��� �-��� ��������� ��� �������� �������� ������
	UE_LOG(LogBaseGeometry, Warning, TEXT("Actor name %s"), *GetName());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Weapons Num: %d, Kill Num: %i"), WeaponsNum, KillNum);
	UE_LOG(LogBaseGeometry, Warning, TEXT("Health: %f"), Health);
	UE_LOG(LogBaseGeometry, Warning, TEXT("IsDead: %d, HasWeapon: %d"), IsDead, static_cast<int>(HasWeapon));
}

void ABaseGeometryActor::printStringTypes()
{
	FString Name = "John Connor";
	UE_LOG(LogBaseGeometry, Display, TEXT("Name: %s"), *Name);

	                                           // �������������� �� int-a � ��� FString
	FString WeaponsNumStr = "Weapons Num = " + FString::FromInt(WeaponsNum);
	                                   // �� float-a � ��� FString  
	FString HealthStr = "Health = " + FString::SanitizeFloat(Health);
	                                    // Ctor FString ������� ����� ������������ �� �������
	FString IsDeadStr = "Is dead = " + FString(IsDead ? "true" : "false");

	FString Stat = FString::Printf(TEXT("\n== All stat == \n %s \n %s \n %s"), *WeaponsNumStr, *HealthStr, *IsDeadStr);
	UE_LOG(LogBaseGeometry, Warning, TEXT("%s"), *Stat);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, Name);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Stat, true, FVector2D(1.5f, 1.5f));
}

void ABaseGeometryActor::printTransform()
{
	FTransform Transform = GetActorTransform();
	FVector Location = Transform.GetLocation();
	FRotator Rotation = Transform.Rotator();
	FVector Scale = Transform.GetScale3D();

	UE_LOG(LogBaseGeometry, Warning, TEXT("Actor name %s"), *GetName());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Transform %s"), *Transform.ToString()/*�������������� � ������*/);
	UE_LOG(LogBaseGeometry, Warning, TEXT("Location %s"), *Location.ToString());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Rotator %s"), *Rotation.ToString());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Scale %s"), *Scale.ToString());

	UE_LOG(LogBaseGeometry, Error, TEXT("Human Transform %s"), *Transform.ToHumanReadableString()/*�������������� � ����� ������������� ������*/);
}

void ABaseGeometryActor::HandleMovement()
{
	switch (GeometryData.MoveType)
	{
	case EMovementType::Sin:
	{
		FVector CurrentLocation = GetActorLocation();
		float time = GetWorld()->GetTimeSeconds(); // GetWorld() - ��������� �� ���������� ������ ���� ����, ������� ���� � ������� ������
												   // GetTimeSeconds() - ���-�� ������ ��������� � ������� ������ ����
		CurrentLocation.Z = InitialLocation.Z + GeometryData.Amplitude * FMath::Sin(GeometryData.Frequency * time);
		SetActorLocation(CurrentLocation); // ������ ������� ��������� ������ � ����
	}
	break;
	case EMovementType::Static:
		break;
	default:
		break;
	}
}

void ABaseGeometryActor::SetColor(const FLinearColor& Color)
{
	// �� ������ �������� ��������� ������ ������� ���������, ������������� ���������� � ���������� ���������
	UMaterialInstanceDynamic* DynMaterial = BaseMesh->CreateAndSetMaterialInstanceDynamic(0);

	if (DynMaterial)
	{
		// ������������� ���� � ��������
		DynMaterial->SetVectorParameterValue("Color", Color);
	}
}

void ABaseGeometryActor::OnTimerFired()
{
	if (++TimerCount <= ManTimerCount)
	{
		const FLinearColor NewColor = FLinearColor::MakeRandomColor(); // ������������� ��������� ����
		UE_LOG(LogBaseGeometry, Display, TEXT("Color to set up: %s"), *NewColor.ToString());
		SetColor(NewColor);

		OnColorChanged.Broadcast(NewColor, GetName());
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		OnTimerFinished.Broadcast(this);
	}
}

