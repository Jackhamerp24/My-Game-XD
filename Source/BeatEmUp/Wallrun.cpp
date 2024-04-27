// Fill out your copyright notice in the Description page of Project Settings.


#include "Wallrun.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"


UWallrun::FSavedMove_Jackhamer::FSavedMove_Jackhamer()
{
}

bool UWallrun::FSavedMove_Jackhamer::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
	float MaxDelta) const
{
	const FSavedMove_Jackhamer* NewJackhamerMove = static_cast<FSavedMove_Jackhamer*>(NewMove.Get());
	
	if (Saved_bWallRunIsRight != NewJackhamerMove->Saved_bWallRunIsRight)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UWallrun::FSavedMove_Jackhamer::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWallRunIsRight = 0;
}

uint8 UWallrun::FSavedMove_Jackhamer::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (Saved_bPressedZippyJump) Result |= FLAG_JumpPressed;

	return Result;
}

void UWallrun::FSavedMove_Jackhamer::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	const UWallrun* CharacterMovement = Cast<UWallrun>(C->GetCharacterMovement());

	Saved_bWallRunIsRight = CharacterMovement->Safe_bWallRunIsRight;
}

void UWallrun::FSavedMove_Jackhamer::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);
	UWallrun* CharacterMovement = Cast<UWallrun>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWallRunIsRight = Saved_bWallRunIsRight;
}

UWallrun::FNetworkPredictionData_Client_Jackhamer::FNetworkPredictionData_Client_Jackhamer(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UWallrun::FNetworkPredictionData_Client_Jackhamer::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Jackhamer());
}

void UWallrun::CallServerMovePacked(const FSavedMove_Character *NewMove, const FSavedMove_Character *PendingMove, const FSavedMove_Character *OldMove)
{
	// Get storage container we'll be using and fill it with movement data
	FCharacterNetworkMoveDataContainer& MoveDataContainer = GetNetworkMoveDataContainer();
	MoveDataContainer.ClientFillNetworkMoveData(NewMove, PendingMove, OldMove);

	// Reset bit writer without affecting allocations
	FBitWriterMark BitWriterReset;
	BitWriterReset.Pop(ZippyServerMoveBitWriter);

	// 'static' to avoid reallocation each invocation
	static FCharacterServerMovePackedBits PackedBits;
	UNetConnection* NetConnection = CharacterOwner->GetNetConnection();	


	{
		// Extract the net package map used for serializing object references.
		ZippyServerMoveBitWriter.PackageMap = NetConnection ? ToRawPtr(NetConnection->PackageMap) : nullptr;
	}

	if (ZippyServerMoveBitWriter.PackageMap == nullptr)
	{
		UE_LOG(LogNetPlayerMovement, Error, TEXT("CallServerMovePacked: Failed to find a NetConnection/PackageMap for data serialization!"));
		return;
	}

	// Serialize move struct into a bit stream
	if (!MoveDataContainer.Serialize(*this, ZippyServerMoveBitWriter, ZippyServerMoveBitWriter.PackageMap) || ZippyServerMoveBitWriter.IsError())
	{
		UE_LOG(LogNetPlayerMovement, Error, TEXT("CallServerMovePacked: Failed to serialize out movement data!"));
		return;
	}

	// Copy bits to our struct that we can NetSerialize to the server.
	PackedBits.DataBits.SetNumUninitialized(ZippyServerMoveBitWriter.GetNumBits());
	
	check(PackedBits.DataBits.Num() >= ZippyServerMoveBitWriter.GetNumBits());
	FMemory::Memcpy(PackedBits.DataBits.GetData(), ZippyServerMoveBitWriter.GetData(), ZippyServerMoveBitWriter.GetNumBytes());

	TotalBitsSent += PackedBits.DataBits.Num();

	// Send bits to server!
	ServerMovePacked_ClientSend(PackedBits);

	MarkForClientCameraUpdate();
}

bool UWallrun::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

float UWallrun::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UWallrun::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void UWallrun::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Wall Run
	//check condition before the movement frame so the character can be in a proper state
	if (IsFalling())
	{
		TryWallRun();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UWallrun::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{


	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

FNetworkPredictionData_Client* UWallrun::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

	if (ClientPredictionData == nullptr)
	{
		UWallrun* MutableThis = const_cast<UWallrun*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Jackhamer(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f; 
	}
	return ClientPredictionData;
}

void UWallrun::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_WallRun:
		PhysWallRun(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

void UWallrun::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	//animation helper
	
	//cheap and efficient to detect whether the wall is on the left or the right when we start a wall run
	//on the simulated proxies
	
	if (IsWallRunning() && GetOwnerRole() == ROLE_SimulatedProxy)
	{
		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector End = Start + UpdatedComponent->GetRightVector() * CapR() * 2;
		auto Params = JackhamerCharacterOwner->GetIgnoreCharacterParams();
		FHitResult WallHit;
		//this value is used on the animation blueprint to make sure which side we are playing the animation on
		Safe_bWallRunIsRight = GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
		
	}
}

bool UWallrun::ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector &Accel, const FVector &ClientWorldLocation, const FVector &RelativeClientLocation, UPrimitiveComponent *ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	if (GetCurrentNetworkMoveData()->NetworkMoveType == FCharacterNetworkMoveData::ENetworkMoveType::NewMove)
	{
		float LocationError = FVector::Dist(UpdatedComponent->GetComponentLocation(), ClientWorldLocation);
		GEngine->AddOnScreenDebugMessage(6, 100.f, FColor::Yellow, FString::Printf(TEXT("Loc: %s"), *ClientWorldLocation.ToString()));
		AccumulatedClientLocationError += LocationError * DeltaTime;
	}


	
	return Super::ServerCheckClientError(ClientTimeStamp, DeltaTime, Accel, ClientWorldLocation, RelativeClientLocation,
	                                     ClientMovementBase,
	                                     ClientBaseBoneName, ClientMovementMode);
}

UWallrun::UWallrun()
{
}

float UWallrun::GetMaxSpeed() const
{
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxSpeed();

	switch (CustomMovementMode)
	{
	case CMOVE_WallRun:
		return MaxWallRunSpeed;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode :("))
		return -1.f;
	}
}

float UWallrun::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
	return Super::GetMaxBrakingDeceleration();

	switch (CustomMovementMode)
	{
	case CMOVE_WallRun:
		return 0.f;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode :("))
		return -1.f;
	}
}

void UWallrun::InitializeComponent()
{
	Super::InitializeComponent();

	JackhamerCharacterOwner = Cast<ABeatEmUpCharacter>(GetOwner());
}

bool UWallrun::TryWallRun()
{
	if (!IsFalling()) return false;

	//Check to make sure the character have enough velocity to perform the wall run (add so the game is more realistic)
	if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
	//Prevent from falling too fast
	if (Velocity.Z < -MaxVerticalWallRunSpeed) return false;
	
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * CapR() * 2;
	FVector RightEnd = Start + UpdatedComponent->GetRightVector() * CapR() * 2;
	auto Params = JackhamerCharacterOwner->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;
	
	// Check Player Height, if the cast hit the ground then no wall running is performed
	if (GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapHH() + MinWallRunHeight), "BlockAll", Params))
	{
		return false;
	}
	
	//Check if the character is appropriately aligned to start or continue a wall run against a surface
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, LeftEnd, "BlockAll", Params);
	// Left Cast
	if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
	{
		Safe_bWallRunIsRight = false;
	}
	// Right Cast
	else
	{
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, RightEnd, "BlockAll", Params);
		if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
		{
			Safe_bWallRunIsRight = true;
		}
		else
		{
			return false;
		}
	}
	//Check to make sure the character have enough velocity to perform the wall run (add so the game is more realistic)
	
	/*This check is more restricted but the above check is for optimization, if we cannot pass the height check above
	then no need to cast or check anything else*/
	
	FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	if (ProjectedVelocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
	
	// Passed all conditions

	//while wall run, we can only move parallel with the wall
	Velocity = ProjectedVelocity;
	
	//Make sure the character's vertical velocity is neither negative (falling)
	//nor excessively high (ascending too quickly)
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, MaxVerticalWallRunSpeed);
	
	SetMovementMode(MOVE_Custom, CMOVE_WallRun); //entering the wall movement mode (YAY)
	UE_LOG(LogTemp, Log, TEXT("Starting WallRun")); //debug helper
		return true;
}

void UWallrun::PhysWallRun(float deltaTime, int32 Iterations)
{
    // Ensure the physics tick is above a minimal threshold to be processed
		if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	
		bJustTeleported = false; //ensuring that each physics simulation cycle handles movement based on actual physics calculations
	float remainingTime = deltaTime;
	// Perform the move
	/* As long as there is remaining time to simulate, the number of iterations hasn't exceeded the maximum,
	 * the character is valid for movement simulation.*/
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		
		// Calculate the simulation time step based on remaining time and iterations
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		// Store the old location for later use in velocity calculation
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Set up for wall detection by raycasting
		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
		FVector End = Safe_bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
		auto Params = JackhamerCharacterOwner->GetIgnoreCharacterParams();
		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
		FHitResult WallHit;
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);

		// Determine if the character wants to pull away from the wall based on acceleration and collision normal
		bool bWantsToPullAway = WallHit.IsValidBlockingHit() && !Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
		
		// Clamp Acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.f;
		// Apply acceleration
		CalcVelocity(timeTick, 0.f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D();
		bool bVelUp = Velocity.Z > 0.f;

		//If the character is pushing backward or not having any input, then more gravity applied,
		//but if push forward (along the wall), then no gravity applied
		Velocity.Z += GetGravityZ() * WallRunGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : TangentAccel) * timeTick;

		// Check if conditions are still favorable for wall running
		if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2) || Velocity.Z < -MaxVerticalWallRunSpeed)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
		
		// Compute move parameters
		const FVector Delta = timeTick * Velocity; // dx = v * dt
		const bool bZeroDelta = Delta.IsNearlyZero();
		if ( bZeroDelta )
		{
			remainingTime = 0.f; // If no movement, end the loop early
		}
		else
		{
			//perform
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * timeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
		}
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick; // v = dx / dt
	}

	//Check if the character can continue wall running or change to falling mode
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
	FVector End = Safe_bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
	auto Params = JackhamerCharacterOwner->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapHH() + MinWallRunHeight * .5f), "BlockAll", Params);
	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit() || Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2))
	{
		SetMovementMode(MOVE_Falling);
	}
}

bool UWallrun::IsServer() const
{
	return CharacterOwner->HasAuthority();
}



bool UWallrun::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsWallRunning();
}

bool UWallrun::DoJump(bool bReplayingMoves)
{
	bool bWasWallRunning = IsWallRunning();
	if (Super::DoJump(bReplayingMoves))
	{
		if (bWasWallRunning)
		{
			//if the wall is on the right, cast a ray out to the right and reverse
			FVector Start = UpdatedComponent->GetComponentLocation();
			FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
			FVector End = Safe_bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
			
			auto Params = JackhamerCharacterOwner->GetIgnoreCharacterParams();
			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
			Velocity += WallHit.Normal * WallJumpOffForce;
		}
		return true;
	}
	return false;
}

void UWallrun::OnClientCorrectionReceived(FNetworkPredictionData_Client_Character &ClientData, float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent *NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode, FVector ServerGravityDirection)
{
		Super::OnClientCorrectionReceived(ClientData, TimeStamp, NewLocation, NewVelocity, NewBase, NewBaseBoneName,
	                                  bHasBase, bBaseRelativePosition,
	                                  ServerMovementMode, ServerGravityDirection);

	CorrectionCount++;
}
