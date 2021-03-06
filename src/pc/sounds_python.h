
#ifndef SOUNDS_PYTHON_H
#define SOUNDS_PYTHON_H

#include <Python.h>

#include "audio_defines.h"
#include "seq_ids.h"

#include "python_helpers.h"

#define PYTHON_SOUND_ADD_CONSTANTS(module) \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_DEFAULT); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_GRASS); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_STONE); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_SPOOKY); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_SNOW); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_ICE); \
    PYTHON_ADD_CONSTANT(module, SOUND_TERRAIN_SAND); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_JUMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_LANDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_STEP); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_STEP_TIPTOE); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_STUCK_IN_GROUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TERRAIN_HEAVY_LANDING); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_JUMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_LANDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_STEP); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_HEAVY_LANDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_CLAP_HANDS_COLD); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_HANGING_STEP); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_QUICKSAND_STEP); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_STEP_TIPTOE); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN430); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN431); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN432); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_SWIM); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN434); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_THROW); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_KEY_SWISH); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_SPIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TWIRL); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_CLIMB_UP_TREE); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_CLIMB_DOWN_TREE); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNK3C); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN43D); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN43E); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_PAT_BACK); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_BRUSH_HAIR); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_CLIMB_UP_POLE); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_BONK); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNSTUCK_FROM_GROUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_HIT); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_HIT_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_HIT_3); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_BONK); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_SHRINK_INTO_BBH); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_SWIM_FAST); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_JUMP_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_LAND_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_METAL_STEP_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNK53); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNK54); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNK55); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_FLYING_FAST); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_TELEPORT); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN458); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_BOUNCE_OFF_OBJECT); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_SIDE_FLIP_UNK); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_READ_SIGN); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNKNOWN45C); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_UNK5D); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_INTRO_UNK45E); \
    PYTHON_ADD_CONSTANT(module, SOUND_ACTION_INTRO_UNK45F); \
    \
    /* Moving Sound Effects */ \
    \
    /* Terrain-dependent moving sounds; a value 0-7 is added to the sound ID before */ \
    /* playing. See higher up for the different terrain types. */ \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_TERRAIN_SLIDE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_TERRAIN_RIDING_SHELL); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_LAVA_BURN); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_SLIDE_DOWN_POLE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_SLIDE_DOWN_TREE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_QUICKSAND_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_SHOCKED); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_FLYING); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_ALMOST_DROWNING); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_AIM_CANNON); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_UNK1A); \
    PYTHON_ADD_CONSTANT(module, SOUND_MOVING_RIDING_SHELL_LAVA); \
    \
    /* Mario Sound Effects */ \
    /* A random number 0-2 is added to the sound ID before playing, producing Yah/Wah/Hoo. */ \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_YAH_WAH_HOO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_HOOHOO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_YAHOO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_UH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_HRMM); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_WAH2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_WHOA); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_EEUH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_ATTACKED); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_OOOF); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_OOOF2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_HERE_WE_GO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_YAWNING); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_SNORING1); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_SNORING2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_WAAAOOOW); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_HAHA); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_HAHA_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_UH2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_UH2_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_ON_FIRE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_DYING); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_PANTING_COLD); \
    \
    /* A random number 0-2 is added to the sound ID before playing. */ \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_PANTING); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_COUGHING1); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_COUGHING2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_COUGHING3); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_PUNCH_YAH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_PUNCH_HOO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_MAMA_MIA); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_OKEY_DOKEY); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_GROUND_POUND_WAH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_DROWNING); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_PUNCH_WAH); \
    \
    /* Mario Sound Effects (US/EU only) */ \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_DEAR_MARIO); \
    \
    /* A random number 0-4 is added to the sound ID before playing, producing one of */ \
    /* Yahoo! (60% chance), Waha! (20%), or Yippee! (20%). */ \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_YAHOO_WAHA_YIPPEE); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_DOH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_GAME_OVER); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_HELLO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_PRESS_START_TO_PLAY); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_TWIRL_BOUNCE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_SNORING3); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_SO_LONGA_BOWSER); \
    PYTHON_ADD_CONSTANT(module, SOUND_MARIO_IMA_TIRED); \
    \
    /* Princess Peach Sound Effects (US/EU only) */ \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_MARIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_POWER_OF_THE_STARS); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_THANKS_TO_YOU); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_THANK_YOU_MARIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_SOMETHING_SPECIAL); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_BAKE_A_CAKE); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_FOR_MARIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_PEACH_MARIO2); \
    \
    /* General Sound Effects */ \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_ACTIVATE_CAP_SWITCH); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_FLAME_OUT); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_OPEN_WOOD_DOOR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CLOSE_WOOD_DOOR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_OPEN_IRON_DOOR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CLOSE_IRON_DOOR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BUBBLES); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_MOVING_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SWISH_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_QUIET_BUBBLE); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_VOLCANO_EXPLOSION); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_QUIET_BUBBLE2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CASTLE_TRAP_OPEN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_WALL_EXPLOSION); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COIN_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_STAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BIG_CLOCK); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_LOUD_POUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_LOUD_POUND2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_POUND1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_POUND2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_POUND3); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_POUND4); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_POUND5); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHORT_POUND6); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_OPEN_CHEST); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CLAM_SHELL1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOX_LANDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOX_LANDING_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN1_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CLAM_SHELL2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CLAM_SHELL3); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_PAINTING_EJECT); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_LEVEL_SELECT_CHANGE); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_PLATFORM); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_DONUT_PLATFORM_EXPLOSION); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOWSER_BOMB_EXPLOSION); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COIN_SPURT); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COIN_SPURT_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COIN_SPURT_EU); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_EXPLOSION6); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNK32); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOAT_TILT1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOAT_TILT2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COIN_DROP); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN3_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN3); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN3_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_PENDULUM_SWING); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CHAIN_CHOMP1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CHAIN_CHOMP2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_DOOR_TURN_KEY); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_MOVING_IN_SAND); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN4_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNKNOWN4); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_MOVING_PLATFORM_SWITCH); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CAGE_OPEN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_QUIET_POUND1_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_QUIET_POUND1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BREAK_BOX); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_DOOR_INSERT_KEY); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_QUIET_POUND2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BIG_POUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNK45); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNK46_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_UNK46); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_CANNON_UP); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_GRINDEL_ROLL); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_EXPLOSION7); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SHAKE_COFFIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_RACE_GUN_SHOT); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_STAR_DOOR_OPEN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_STAR_DOOR_CLOSE); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_POUND_ROCK); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_STAR_APPEARS); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_COLLECT_1UP); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BUTTON_PRESS_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BUTTON_PRESS); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BUTTON_PRESS_2_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BUTTON_PRESS_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_ELEVATOR_MOVE); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_ELEVATOR_MOVE_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SWISH_AIR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SWISH_AIR_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_HAUNTED_CHAIR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SOFT_LANDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_HAUNTED_CHAIR_MOVE); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOWSER_PLATFORM); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOWSER_PLATFORM_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_HEART_SPIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_POUND_WOOD_POST); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_WATER_LEVEL_TRIG); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SWITCH_DOOR_OPEN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_RED_COIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BIRDS_FLY_AWAY); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_METAL_POUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOING1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOING2_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOING2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_YOSHI_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_ENEMY_ALERT1); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_YOSHI_TALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_SPLATTERING); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOING3); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_GRAND_STAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_GRAND_STAR_JUMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_BOAT_ROCK); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL_VANISH_SFX); \
    \
    /* Environment Sound Effects */ \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_WATERFALL1); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_WATERFALL2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_ELEVATOR1); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_DRONING1); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_DRONING2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_WIND1); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_MOVING_SAND_SNOW); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_UNK07); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_ELEVATOR2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_UNKNOWN2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_BOAT_ROCKING1); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_ELEVATOR3); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_ELEVATOR4); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_ELEVATOR4_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_MOVINGSAND); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_MERRY_GO_ROUND_CREAKING); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_WIND2); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_UNK12); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_SLIDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_STAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_UNKNOWN4); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_WATER_DRAIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_METAL_BOX_PUSH); \
    PYTHON_ADD_CONSTANT(module, SOUND_ENV_SINK_QUICKSAND); \
    \
    /* Object Sound Effects */ \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SUSHI_SHARK_WATER_SOUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MRI_SHOOT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BABY_PENGUIN_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_TAIL_PICKUP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_DEFEATED); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_SPINNING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_INHALING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BIG_PENGUIN_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOO_BOUNCE_TOP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOO_LAUGH_SHORT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_THWOMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_CANNON1); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_CANNON2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_CANNON3); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_JUMP_WALK_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UNKNOWN2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MRI_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_POUNDING1); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_POUNDING1_HIGHPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WHOMP_LOWPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KING_BOBOMB); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BULLY_METAL); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BULLY_EXPLODE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BULLY_EXPLODE_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_POUNDING_CANNON); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BULLY_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UNKNOWN3); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UNKNOWN4); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BABY_PENGUIN_DIVE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_GOOMBA_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UKIKI_CHATTER_LONG); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MONTY_MOLE_ATTACK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_EVIL_LAKITU_THROW); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UNK23); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_DYING_ENEMY1); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_CANNON4); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_DYING_ENEMY2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOBOMB_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SOMETHING_LANDING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_DIVING_IN_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SNOW_SAND1); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SNOW_SAND2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_DEFAULT_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BIG_PENGUIN_YELL); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WATER_BOMB_BOUNCING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_GOOMBA_ALERT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WIGGLER_JUMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_STOMPED); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UNKNOWN6); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_DIVING_INTO_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_PIRANHA_PLANT_SHRINK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KOOPA_THE_QUICK_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KOOPA_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BULLY_WALKING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_DORRIE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_LAUGH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UKIKI_CHATTER_SHORT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UKIKI_CHATTER_IDLE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UKIKI_STEP_DEFAULT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_UKIKI_STEP_LEAVES); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KOOPA_TALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KOOPA_DAMAGE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KLEPTO1); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KLEPTO2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KING_BOBOMB_TALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KING_BOBOMB_JUMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KING_WHOMP_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOO_LAUGH_LONG); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_EEL); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_EEL_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_EYEROK_SHOW_EYE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MR_BLIZZARD_ALERT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SNUFIT_SHOOT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SKEETER_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WALKING_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BIRD_CHIRP3); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_PIRANHA_PLANT_APPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_FLAME_BLOWN); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MAD_PIANO_CHOMPING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOBOMB_BUDDY_TALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SPINY_UNK59); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WIGGLER_HIGH_PITCH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_HEAVEHO_TOSSED); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WIGGLER_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BOWSER_INTRO_LAUGH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_ENEMY_DEATH_HIGH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_ENEMY_DEATH_LOW); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SWOOP_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_KOOPA_FLYGUY_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_POKEY_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SNOWMAN_BOUNCE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SNOWMAN_EXPLODE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_POUNDING_LOUD); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MIPS_RABBIT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_MIPS_RABBIT_WATER); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_EYEROK_EXPLODE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_CHUCKYA_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WIGGLER_TALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WIGGLER_ATTACKED); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_WIGGLER_LOW_PITCH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_SNUFIT_SKEETER_DEATH); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_BUBBA_CHOMP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ_ENEMY_DEFEAT_SHRINK); \
    \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_BOWSER_SPIT_FIRE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_UNK01); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_LAKITU_FLY); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_LAKITU_FLY_HIGHPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_AMP_BUZZ); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_BLOW_FIRE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_BLOW_WIND); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_ROUGH_SLIDE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_HEAVEHO_MOVE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_UNK07); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_BOBOMB_LIT_FUSE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_HOWLING_WIND); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_CHUCKYA_MOVE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_PEACH_TWINKLE); \
    PYTHON_ADD_CONSTANT(module, SOUND_AIR_CASTLE_OUTDOORS_AMBIENT); \
    \
    /* Menu Sound Effects */ \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CHANGE_SELECT); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_REVERSE_PAUSE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_PAUSE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_PAUSE_HIGHPRIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_PAUSE_2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_MESSAGE_APPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_MESSAGE_DISAPPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CAMERA_ZOOM_IN); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CAMERA_ZOOM_OUT); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_PINCH_MARIO_FACE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_LET_GO_MARIO_FACE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_HAND_APPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_HAND_DISAPPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_UNK0C); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_POWER_METER); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CAMERA_BUZZ); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CAMERA_TURN); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_UNK10); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CLICK_FILE_SELECT); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_MESSAGE_NEXT_PAGE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_COIN_ITS_A_ME_MARIO); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_YOSHI_GAIN_LIVES); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_ENTER_PIPE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_EXIT_PIPE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_BOWSER_LAUGH); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_ENTER_HOLE); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CLICK_CHANGE_VIEW); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CAMERA_UNUSED1); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_CAMERA_UNUSED2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_MARIO_CASTLE_WARP); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_STAR_SOUND); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_THANK_YOU_PLAYING_MY_GAME); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_READ_A_SIGN); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_EXIT_A_SIGN); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_MARIO_CASTLE_WARP2); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_STAR_SOUND_OKEY_DOKEY); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_STAR_SOUND_LETS_A_GO); \
    \
    /* US/EU only; an index between 0-7 or 0-4 is added to the sound ID before */ \
    /* playing, producing the same sound with different pitch. */ \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_COLLECT_RED_COIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_MENU_COLLECT_SECRET); \
    \
    /* Channel 8 loads sounds from the same place as channel 3, making it possible */ \
    /* to play two channel 3 sounds at once (since just one sound from each channel */ \
    /* can play at a given time). */ \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_BOBOMB_EXPLOSION); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_PURPLE_SWITCH); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_ROTATING_BLOCK_CLICK); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_SPINDEL_ROLL); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_PYRAMID_TOP_SPIN); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_PYRAMID_TOP_EXPLOSION); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_BIRD_CHIRP2); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_SWITCH_TICK_FAST); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_SWITCH_TICK_SLOW); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_STAR_APPEARS); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_ROTATING_BLOCK_ALERT); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_BOWSER_EXPLODE); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_BOWSER_KEY); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_1UP_APPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_GENERAL2_RIGHT_ANSWER); \
    \
    /* Channel 9 loads sounds from the same place as channel 5. */ \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BOWSER_ROAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_PIRANHA_PLANT_BITE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_PIRANHA_PLANT_DYING); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BULLY_ATTACKED); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_KING_BOBOMB_DAMAGE); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_SCUTTLEBUG_WALK); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_SCUTTLEBUG_ALERT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BABY_PENGUIN_YELL); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_SWOOP); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BIRD_CHIRP1); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_LARGE_BULLY_ATTACKED); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_EYEROK_SOUND_SHORT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_WHOMP_SOUND_SHORT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_EYEROK_SOUND_LONG); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BOWSER_TELEPORT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_MONTY_MOLE_APPEAR); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_BOSS_DIALOG_GRUNT); \
    PYTHON_ADD_CONSTANT(module, SOUND_OBJ2_MRI_SPINNING); \
    \
    PYTHON_ADD_CONSTANT(module, SEQ_VARIATION); \
    \
    PYTHON_ADD_CONSTANT(module, SEQ_SOUND_PLAYER); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_COLLECT_STAR); \
    PYTHON_ADD_CONSTANT(module, SEQ_MENU_TITLE_SCREEN); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_GRASS); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_INSIDE_CASTLE); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_WATER); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_HOT); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_BOSS_KOOPA); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_SNOW); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_SLIDE); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_SPOOKY); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_PIRANHA_PLANT); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_UNDERGROUND); \
    PYTHON_ADD_CONSTANT(module, SEQ_MENU_STAR_SELECT); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_POWERUP); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_METAL_CAP); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_KOOPA_MESSAGE); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_KOOPA_ROAD); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_HIGH_SCORE); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_MERRY_GO_ROUND); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_RACE); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_STAR_SPAWN); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_BOSS); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_COLLECT_KEY); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_ENDLESS_STAIRS); \
    PYTHON_ADD_CONSTANT(module, SEQ_LEVEL_BOSS_KOOPA_FINAL); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_CREDITS); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_SOLVE_PUZZLE); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_TOAD_MESSAGE); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_PEACH_MESSAGE); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_INTRO); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_VICTORY); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_ENDING); \
    PYTHON_ADD_CONSTANT(module, SEQ_MENU_FILE_SELECT); \
    PYTHON_ADD_CONSTANT(module, SEQ_EVENT_CUTSCENE_LAKITU); \
    PYTHON_ADD_CONSTANT(module, SEQ_COUNT);

PyObject* PyInit_sounds(void);

#endif /* SOUNDS_PYTHON_H */
