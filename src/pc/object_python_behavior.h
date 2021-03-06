
#ifndef OBJECT_PYTHON_BEHAVIOR_H
#define OBJECT_PYTHON_BEHAVIOR_H

#include "types.h"

#include <Python.h>

#include "python_helpers.h"
#include "game/object_list_processor.h"
#include "behavior_data.h"

#define PYCAPSULE_TYPE_BEHAVIOR "objects.Behavior._native_behavior"

#define PYTHON_ENCAPSULE_BEHAVIOR(bhv, ret_on_fail) \
    PYTHON_ENCAPSULE(bhv, PYCAPSULE_TYPE_BEHAVIOR, gLoggerBehavior, ret_on_fail)

#define PYTHON_DECAPSULE_BEHAVIOR(cap, ret_on_fail) \
    PYTHON_DECAPSULE(cap, PYCAPSULE_TYPE_BEHAVIOR, BehaviorScript, gLoggerBehavior, ret_on_fail)

#define PYTHON_CALL_BEHAVIOR(args) \
    (struct _PyObjectBehaviorClass *)PyObject_CallObject( \
        (PyObject *)&PyObjectBehaviorType, args);

#define PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhv) \
    pBhvNative = PYTHON_ENCAPSULE_BEHAVIOR(bhv, return NULL) \
    Py_INCREF(pBhvNative); \
    pBhvArgs = PyTuple_New(1); \
    PyTuple_SetItem(pBhvArgs, 0, pBhvNative); \
    /*pBhvName = PyUnicode_FromString(#bhv); */ \
    /* PyTuple_SetItem(pBhvArgs, 1, pBhvName); */ \
    pBhv = PYTHON_CALL_BEHAVIOR(pBhvArgs); \
    if (0 > PyModule_AddObject(module, #bhv, (PyObject *)pBhv)) { \
        Py_XDECREF(pBhv); \
        Py_XDECREF(pBhvArgs); \
        return NULL; \
    } \
    Py_XDECREF(pBhvArgs);

#define OBJECTS_ADD_BEHAVIORS(module) \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStarDoor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMrI); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMrIBody); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMrIParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPurpleParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGiantPole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPoleGrabbing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvThiHugeIslandTop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvThiTinyIslandTop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCapSwitchBase); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCapSwitch); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKingBobomb); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombAnchorMario); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaChestBottom); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaChestLid); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBubbleParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBubbleMaybe); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallWaterWave); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallWaterWave398); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterAirBubble); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPlungeBubble); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallParticleSnow); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallParticleBubbles); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFishGroup); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCannon); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCannonBarrel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCannonBaseUnused); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChuckya); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChuckyaAnchorMario); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRotatingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTower); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBulletBillCannon); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfBreakableWallRight); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfBreakableWallLeft); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKickableBoard); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTowerDoor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRotatingCounterClockwise); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfRotatingWoodenPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKoopaShellUnderwater); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvExitPodiumWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFadingWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarpPipe); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhitePuffExplosion); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSpawnedStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSpawnedStarNoLevelExit); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMrIBlueCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCoinInsideBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCoinFormationSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCoinFormation); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvOneCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvYellowCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTemporaryYellowCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvThreeCoinsSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTenCoinsSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSingleCoinGetsSpawned); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCoinSparkles); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGoldenCoinSparkles); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWallTinyStarParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvVertStarParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPoundTinyStarParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHorStarParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPunchTinyTriangle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTriangleParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDoorWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDoor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGrindel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvThwomp2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvThwomp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTumblingBridgePlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfTumblingBridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBbhTumblingBridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllTumblingBridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvAnotherElavator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRrElevatorPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHmcElevatorPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterMist); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBreathParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBreakBoxTriangle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterMist2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMistCircParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDirtParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnowParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWind); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEndToad); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEndPeach); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUnusedParticleSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUkiki); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUkikiCageChild); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUkikiCageStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUkikiCage); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBitfsSinkingPlatforms); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBitfsSinkingCagePlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDddMovingPole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBitfsTiltingInvertedPyramid); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSquishablePlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCutOutObject); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaMovingFlamesSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaMovingFlames); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRrRotatingBridgePlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlamethrower); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlamethrowerFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBouncingFireball); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBouncingFireballFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserShockWave); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFireParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlackSmokeMario); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlackSmokeBowser); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlackSmokeUpward); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaFishSplashSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSpindrift); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTowerPlatformGroup); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfSlidingTowerPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfElevatorTowerPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfSolidTowerPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLeafParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreeSnow); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreeLeaf); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvAnotherTiltingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSquarishPathMoving); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPiranhaPlantBubble); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPiranhaPlantWakingBubbles); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFloorSwitchAnimatesObject); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFloorSwitchGrills); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFloorSwitchHardcodedModel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFloorSwitchHiddenObjects); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenObject); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBreakableBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPushableMetalBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHeaveHo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHeaveHoThrowMario); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCcmTouchedStarSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUnusedPoundablePlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaTrampolineTop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaTrampolineSpring); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJumpingBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBooCage); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStub); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvIgloo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserKey); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGrandStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaBooKey); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvAlphaBooKey); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBulletBill); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhitePuffSmoke); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserTailAnchor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowser); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserBodyAnchor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserFlameSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTiltingBowserLavaPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFallingBowserPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlueBowserFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlameFloatingLanding); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlueFlamesGroup); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlameBouncing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlameMovingForwardGrowing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlameBowser); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlameLargeBurningOut); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlueFish); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTankFishGroup); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCheckerboardElevatorGroup); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCheckerboardPlatformSub); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserKeyUnlockDoor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserKeyCourseExit); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvInvisibleObjectsUnderBridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterLevelPillar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDddWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMoatGrills); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvClockMinuteHand); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvClockHourHand); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMacroUkiki); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStub1D0C); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllRotatingHexagonalPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllSinkingRockBlock); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStub1D70); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllMovingOctagonalMeshPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnowBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllRotatingBlockWithFireBars); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllRotatingHexFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllWoodPiece); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllFloatingWoodBridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvVolcanoFlames); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllRotatingHexagonalRing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllSinkingRectangularPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllSinkingSquarePlatforms); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllTiltingInvertedPyramid); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKoopaShell); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKoopaShellFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvToxBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPiranhaPlant); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllHexagonalMesh); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllBowserPuzzlePiece); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllBowserPuzzle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTuxiesMother); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPenguinBaby); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallPenguin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFish2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFish3); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLargeFishGroup); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFish); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWdwExpressElevator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWdwExpressElevatorPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChirpChirp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChirpChirpUnused); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBub); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvExclamationBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRotatingExclamationMark); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSoundSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRockSolid); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserSubDoor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowsersSub); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSushiShark); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSushiSharkCollisionChild); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJrbSlidingBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvShipPart3); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvInSunkenShip3); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSunkenShipPart); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSunkenShipPart2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvInSunkenShip); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvInSunkenShip2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMistParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhitePuff1); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhitePuff2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhitePuffSmoke2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPurpleSwitchHiddenBoxes); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlueCoinSwitch); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenBlueCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvOpenableCageDoor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvOpenableGrill); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterLevelDiamond); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvInitializeChangingWaterLevel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTweesterSandParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTweester); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMerryGoRoundBooManager); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPlaysMusicTrackWhenTouched); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvAnimatedTexture); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBooInCastle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBooWithCage); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBalconyBigBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMerryGoRoundBigBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGhostHuntBigBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCourtyardBooTriplet); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMerryGoRoundBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGhostHuntBoo); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenStaircaseStep); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBooBossSpawnedBridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBbhTiltingTrapPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHauntedBookshelf); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMeshElevator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMerryGoRound); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvInsideCannon); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaBowserAnchor); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStaticCheckeredPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStaticObject); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCastleFloorTrap); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFloorTrapInCastle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTree); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSparkle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSparkleSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSparkleParticleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvScuttlebug); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvScuttlebugSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhompKingBoss); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallWhomp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterSplash); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterDroplet); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterDropletSplash); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBubbleSplash); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvIdleWaterWave); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvObjectWaterSplash); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvShallowWaterWave); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvShallowWaterSplash); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvObjectWaveTrail); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaveTrail); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTinyStrongWindParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStrongWindParticle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSLSnowmanWind); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSLWalkingPenguin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvYellowBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMario); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvToadMessage); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUnlockDoorStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps60); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps64); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps68); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps6C); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDeathWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps74); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps78); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps7C); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPaintingDeathWarp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps84); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps88); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps8C); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps90); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWarps94); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRandomAnimatedTexture); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvYellowBackgroundInMenu); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMenuButton); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMenuButtonManager); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvActSelectorStarType); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvActSelector); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMovingYellowCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMovingBlueCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlueCoinSliding); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBlueCoinJumping); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSeaweed); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSeaweedBundle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobomb); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombFuseSmoke); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombBuddy); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombBuddyOpensCannon); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCannonClosed); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWhirlpool); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJetStream); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMessagePanel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSignOnWall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHomingAmp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCirclingAmp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvButterfly); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHoot); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBetaHoldableObject); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCarrySomething1); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCarrySomething2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCarrySomething3); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCarrySomething4); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCarrySomething5); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCarrySomething6); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvObjectBubble); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvObjectWaterWave); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvExplosion); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombBullyDeathSmoke); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmoke); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombExplosionBubble); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobombExplosionBubble3600); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRespawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallBully); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBigBully); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBigBullyWithMinions); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallChillBully); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBigChillBully); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJetStreamRingSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJetStreamWaterRing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMantaRayWaterRing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMantaRayRingManager); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserBomb); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserBombExplosion); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserBombSmoke); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCelebrationStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCelebrationStarSparkle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStarKeyCollectionPuffSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllDrawbridgeSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllDrawbridge); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallBomp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLargeBomp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWfSlidingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMoneybag); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMoneybagHidden); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPitBowlingBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFreeBowlingBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowlingBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTtmBowlingBallSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBobBowlingBallSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvThiBowlingBallSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRrCruiserWing); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSpindel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSslMovingPyramidWall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPyramidElevator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPyramidElevatorTrajectoryMarkerBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPyramidTop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPyramidTopFragment); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPyramidPillarTouchDetector); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterfallSoundLoop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvVolcanoSoundLoop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCastleFlagWaving); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBirdsSoundLoop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvAmbientSounds); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSandSoundLoop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenAt120Stars); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnowmansBottom); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnowmansHead); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnowmansBodyCheckpoint); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBigSnowmanWhole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBigBoulder); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBigBoulderGenerator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWingCap); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMetalCap); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvNormalCap); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvVanishCap); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvStarSpawnCoordinates); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenRedCoinStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRedCoin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBowserCourseRedCoinStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenStar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHiddenStarTrigger); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTtmRollingLog); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllVolcanoFallingTrap); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvLllRollingLog); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhv1upWalking); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhv1upRunningAway); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhv1upSliding); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhv1Up); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhv1upJumpOnApproach); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHidden1up); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHidden1upTrigger); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHidden1upInPole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHidden1upInPoleTrigger); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHidden1upInPoleSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvControllablePlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvControllablePlatformSub); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBreakableBoxSmall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSlidingSnowMound); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnowMoundSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWdwSquareFloatingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWdwRectangularFloatingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJrbFloatingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvArrowLift); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvOrangeNumber); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMantaRay); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFallingPillar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFallingPillarHitbox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPillarBase); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvJrbFloatingBox); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDecorativePendulum); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreasureChestsShip); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreasureChestsJrb); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreasureChests); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreasureChestBottom); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTreasureChestTop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMips); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvYoshi); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKoopa); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKoopaRaceEndpoint); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKoopaFlag); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPokey); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPokeyBodyPart); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSwoop); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlyGuy); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGoomba); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvGoombaTripletSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChainChomp); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChainChompChainPart); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWoodenPost); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvChainChompGate); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWigglerHead); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWigglerBody); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEnemyLakitu); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCameraLakitu); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCloud); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCloudPart); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSpiny); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMontyMole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMontyMoleHole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMontyMoleRock); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPlatformOnTrack); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTrackBall); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSeesawPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFerrisWheelAxle); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFerrisWheelPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterBombSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterBomb); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterBombShadow); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCRotatingSolid); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCPendulum); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCTreadmill); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCMovingBar); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCCog); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCPitBlock); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCElevator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTC2DRotator); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTTCSpinner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMrBlizzard); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMrBlizzardSnowball); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSlidingPlatform2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvOctagonalPlatformRotating); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvAnimatesOnFloorSwitchPress); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvActivatedBackAndForthPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRecoveryHeart); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvWaterBombCannon); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCannonBarrelBubbles); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUnagi); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUnagiSubobject); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDorrie); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHauntedChair); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvMadPiano); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlyingBookend); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBookendSpawn); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHauntedBookshelfManager); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBookSwitch); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFirePiranhaPlant); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSmallPiranhaFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFireSpitter); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvFlyguyFlame); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnufit); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSnufitBalls); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvHorizontalGrindel); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEyerokBoss); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEyerokHand); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvKlepto); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBird); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRacingPenguin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPenguinRaceFinishLine); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvPenguinRaceShortcutCheck); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCoffinSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvCoffin); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvClamShell); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSkeeter); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSkeeterWave); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvSwingPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDonutPlatformSpawner); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDonutPlatform); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvDDDPole); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvRedCoinStarMarker); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvTripletButterfly); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBubba); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBeginningLakitu); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvBeginningPeach); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEndBirds1); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvEndBirds2); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvIntroScene); \
    PYTHON_WRAP_NATIVE_BEHAVIOR(module, bhvUnusedFakeStar);

#define ADD_OBJLIST_CONSTANTS(module) \
    PyModule_AddIntConstant(module, "OBJ_LIST_PLAYER", OBJ_LIST_PLAYER); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNUSED_1", OBJ_LIST_UNUSED_1); \
    PyModule_AddIntConstant(module, "OBJ_LIST_DESTRUCTIVE", OBJ_LIST_DESTRUCTIVE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNUSED_3", OBJ_LIST_UNUSED_3); \
    PyModule_AddIntConstant(module, "OBJ_LIST_GENACTOR", OBJ_LIST_GENACTOR); \
    PyModule_AddIntConstant(module, "OBJ_LIST_PUSHABLE", OBJ_LIST_PUSHABLE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_LEVEL", OBJ_LIST_LEVEL); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNUSED_7", OBJ_LIST_UNUSED_7); \
    PyModule_AddIntConstant(module, "OBJ_LIST_DEFAULT", OBJ_LIST_DEFAULT); \
    PyModule_AddIntConstant(module, "OBJ_LIST_SURFACE", OBJ_LIST_SURFACE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_POLELIKE", OBJ_LIST_POLELIKE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_SPAWNER", OBJ_LIST_SPAWNER); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNIMPORTANT", OBJ_LIST_UNIMPORTANT); \
    PyModule_AddIntConstant(module, "NUM_OBJ_LISTS", NUM_OBJ_LISTS);

struct _PyObjectBehaviorClass;

BehaviorScript *
PyObjectBehavior_get_native(struct _PyObjectBehaviorClass *self);

#endif /* OBJECT_PYTHON_BEHAVIOR_H */
