// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGeometryActor.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

// собственная категория логирования
// данный макрос отвечает за создание локальной категория логирования
// т. е. доступна только в рамках данного cpp-файла
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
	
	InitialLocation = GetActorLocation(); // положение Актора впространстве

	//printTypes();
	//printStringTypes();
	//printTransform();

	SetColor(GeometryData.Color);

	// данная ф-ция возвращает объкт TimerManager
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseGeometryActor::OnTimerFired, GeometryData.TimerRate, true);
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

	// каждый Актор имеет GetName(). Эта ф-ция возращает имя текущего инстанса Актора
	UE_LOG(LogBaseGeometry, Warning, TEXT("Actor name %s"), *GetName());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Weapons Num: %d, Kill Num: %i"), WeaponsNum, KillNum);
	UE_LOG(LogBaseGeometry, Warning, TEXT("Health: %f"), Health);
	UE_LOG(LogBaseGeometry, Warning, TEXT("IsDead: %d, HasWeapon: %d"), IsDead, static_cast<int>(HasWeapon));
}

void ABaseGeometryActor::printStringTypes()
{
	FString Name = "John Connor";
	UE_LOG(LogBaseGeometry, Display, TEXT("Name: %s"), *Name);

	                                           // переобразовует из int-a в тип FString
	FString WeaponsNumStr = "Weapons Num = " + FString::FromInt(WeaponsNum);
	                                   // из float-a в тип FString  
	FString HealthStr = "Health = " + FString::SanitizeFloat(Health);
	                                    // Ctor FString создаст сроку взависимости от условия
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
	UE_LOG(LogBaseGeometry, Warning, TEXT("Transform %s"), *Transform.ToString()/*преобразование в строку*/);
	UE_LOG(LogBaseGeometry, Warning, TEXT("Location %s"), *Location.ToString());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Rotator %s"), *Rotation.ToString());
	UE_LOG(LogBaseGeometry, Warning, TEXT("Scale %s"), *Scale.ToString());

	UE_LOG(LogBaseGeometry, Error, TEXT("Human Transform %s"), *Transform.ToHumanReadableString()/*преобразование в более информативную строку*/);
}

void ABaseGeometryActor::HandleMovement()
{
	switch (GeometryData.MoveType)
	{
	case EMovementType::Sin:
	{
		FVector CurrentLocation = GetActorLocation();
		float time = GetWorld()->GetTimeSeconds(); // GetWorld() - указатель на глобальный объект мира игры, который есть у каждого актора
												   // GetTimeSeconds() - кол-во секунд прошедшие с момента старта игры
		CurrentLocation.Z = InitialLocation.Z + GeometryData.Amplitude * FMath::Sin(GeometryData.Frequency * time);
		SetActorLocation(CurrentLocation); // меняет текущее положение Актора в мире
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
	// на основе обычного материала создаёт инстанс материала, устанавливает компоненту и возвращает указатель
	UMaterialInstanceDynamic* DynMaterial = BaseMesh->CreateAndSetMaterialInstanceDynamic(0);

	if (DynMaterial)
	{
		// устанавливает цвет в параметр
		DynMaterial->SetVectorParameterValue("Color", Color);
	}
}

void ABaseGeometryActor::OnTimerFired()
{
	if (++TimerCount <= ManTimerCount)
	{
		const FLinearColor NewColor = FLinearColor::MakeRandomColor(); // устанавливает рандомный цвет
		UE_LOG(LogBaseGeometry, Display, TEXT("Color to set up: %s"), *NewColor.ToString());
		SetColor(NewColor);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}

