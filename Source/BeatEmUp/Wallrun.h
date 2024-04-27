// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BeatEmUpCharacter.h"
#include "Wallrun.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_WallRun		UMETA(DisplayName = "Wall Run"),
};

UCLASS()
class BEATEMUP_API UWallrun : public UCharacterMovementComponent
{
	GENERATED_BODY()
	class FSavedMove_Jackhamer : public FSavedMove_Character
	{
	public:
		uint8 Saved_bPressedZippyJump:1;
		uint8 Saved_bWallRunIsRight:1;
		
		FSavedMove_Jackhamer();

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		
	};
	class FNetworkPredictionData_Client_Jackhamer : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Jackhamer(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;
			
		virtual FSavedMovePtr AllocateNewMove() override;
	};
	// Wall Run
	UPROPERTY(EditDefaultsOnly) float MinWallRunSpeed=200.f;
	UPROPERTY(EditDefaultsOnly) float MaxWallRunSpeed=800.f;
	UPROPERTY(EditDefaultsOnly) float MaxVerticalWallRunSpeed=200.f;
	UPROPERTY(EditDefaultsOnly) float WallRunPullAwayAngle=75;
	UPROPERTY(EditDefaultsOnly) float WallAttractionForce = 200.f;
	UPROPERTY(EditDefaultsOnly) float MinWallRunHeight=50.f;
	UPROPERTY(EditDefaultsOnly) UCurveFloat* WallRunGravityScaleCurve;
	UPROPERTY(EditDefaultsOnly) float WallJumpOffForce = 300.f;

	//Transient
	UPROPERTY(Transient) ABeatEmUpCharacter* JackhamerCharacterOwner;
	bool Safe_bWallRunIsRight;
	
	float AccumulatedClientLocationError=0.f;
	int TotalBitsSent=0;
	int CorrectionCount=0;
public:
	UWallrun();

protected:
	virtual void InitializeComponent() override;

	
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	//override the way that the player can jump (player can also jump to a wall run
	virtual bool CanAttemptJump() const override;
	//add horizontal force if the player is doing the wall run
	virtual bool DoJump(bool bReplayingMoves) override;

protected:
	virtual void OnClientCorrectionReceived(FNetworkPredictionData_Client_Character& ClientData, float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode,  FVector ServerGravityDirection) override;

public:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

protected:
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual bool ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

	FNetBitWriter ZippyServerMoveBitWriter;

	virtual void CallServerMovePacked(const FSavedMove_Character* NewMove, const FSavedMove_Character* PendingMove, const FSavedMove_Character* OldMove) override;

private:
	//Wall Run
	bool TryWallRun();
	void PhysWallRun(float deltatime, int32 Iterations); //update the wall run over every frame
	
private:
	bool IsServer() const;
	float CapR() const;
	float CapHH() const;
	
public:
	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	
	UFUNCTION(BlueprintPure) bool IsWallRunning() const { return IsCustomMovementMode(CMOVE_WallRun); }
	UFUNCTION(BlueprintPure) bool WallRunningIsRight() const { return Safe_bWallRunIsRight; }
};