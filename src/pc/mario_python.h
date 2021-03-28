
#ifndef MARIO_PYTHON_H
#define MARIO_PYTHON_H

#include "types.h"

#include <ultra64.h>

#include <Python.h>

#include "python_helpers.h"

#define PYCAPSULE_TYPE_MARIO "mario.MarioState._native_state"

#define PYTHON_ENCAPSULE_MARIO(obj, ret_on_fail) \
    PYTHON_ENCAPSULE(obj, PYCAPSULE_TYPE_MARIO, ret_on_fail)
    
#define PYTHON_DECAPSULE_MARIO(cap, ret_on_fail) \
    PYTHON_DECAPSULE(cap, PYCAPSULE_TYPE_MARIO, struct MarioState, ret_on_fail)


#define INT_GROUND_POUND_OR_TWIRL (1 << 0) // 0x01
#define INT_PUNCH                 (1 << 1) // 0x02
#define INT_KICK                  (1 << 2) // 0x04
#define INT_TRIP                  (1 << 3) // 0x08
#define INT_SLIDE_KICK            (1 << 4) // 0x10
#define INT_FAST_ATTACK_OR_SHELL  (1 << 5) // 0x20
#define INT_HIT_FROM_ABOVE        (1 << 6) // 0x40
#define INT_HIT_FROM_BELOW        (1 << 7) // 0x80

#define PYTHON_MARIO_ADD_CONSTANT(module, constant) \
    PyModule_AddIntConstant(module, #constant, constant);

#define PYTHON_MARIO_ADD_COMPOUND_CONSTANT(module, constant, value) \
    PyModule_AddIntConstant(module, #constant, value);

#define PYTHON_MARIO_ADD_LAYER_CONSTANTS(module) \
    /* Layers */ \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_FORCE); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_OPAQUE); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_OPAQUE_DECAL); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_OPAQUE_INTER); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_ALPHA); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_TRANSPARENT); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_TRANSPARENT_DECAL); \
    PYTHON_MARIO_ADD_CONSTANT(module, LAYER_TRANSPARENT_INTER); \

#define PYTHON_MARIO_ADD_INPUT_CONSTANTS(module) \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_NONZERO_ANALOG); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_A_PRESSED); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_OFF_FLOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_ABOVE_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_FIRST_PERSON); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_UNKNOWN_5); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_SQUISHED); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_A_DOWN); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_IN_POISON_GAS); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_IN_WATER); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_UNKNOWN_10); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_INTERACT_OBJ_GRABBABLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_UNKNOWN_12); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_B_PRESSED); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_Z_DOWN); \
    PYTHON_MARIO_ADD_CONSTANT(module, INPUT_Z_PRESSED); \

#define PYTHON_MARIO_ADD_STEP_CONSTANTS(module) \
    PYTHON_MARIO_ADD_CONSTANT(module, GROUND_STEP_LEFT_GROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, GROUND_STEP_NONE); \
    PYTHON_MARIO_ADD_CONSTANT(module, GROUND_STEP_HIT_WALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, GROUND_STEP_HIT_WALL_STOP_QSTEPS); \
    PYTHON_MARIO_ADD_CONSTANT(module, GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_CHECK_LEDGE_GRAB); \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_CHECK_HANG); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_NONE); \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_LANDED); \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_HIT_WALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_GRABBED_LEDGE); \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_GRABBED_CEILING); \
    PYTHON_MARIO_ADD_CONSTANT(module, AIR_STEP_HIT_LAVA_WALL); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, WATER_STEP_NONE); \
    PYTHON_MARIO_ADD_CONSTANT(module, WATER_STEP_HIT_FLOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, WATER_STEP_HIT_CEILING); \
    PYTHON_MARIO_ADD_CONSTANT(module, WATER_STEP_CANCELLED); \
    PYTHON_MARIO_ADD_CONSTANT(module, WATER_STEP_HIT_WALL); \

#define PYTHON_MARIO_ADD_PARTICLE_CONSTANTS(module) \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_DUST); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_VERTICAL_STAR); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_2); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_SPARKLES); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_HORIZONTAL_STAR); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_BUBBLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_WATER_SPLASH); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_IDLE_WATER_WAVE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_SHALLOW_WATER_WAVE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_PLUNGE_BUBBLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_WAVE_TRAIL); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_FIRE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_SHALLOW_WATER_SPLASH); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_LEAF); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_SNOW); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_DIRT); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_MIST_CIRCLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_BREATH); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_TRIANGLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, PARTICLE_19); \

#define PYTHON_MARIO_ADD_MARIO_CONSTANTS(module) \
    PYTHON_MARIO_ADD_CONSTANT(module, MODEL_STATE_NOISE_ALPHA); \
    PYTHON_MARIO_ADD_CONSTANT(module, MODEL_STATE_METAL); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_NORMAL_CAP); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_VANISH_CAP); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_METAL_CAP); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_WING_CAP); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_CAP_ON_HEAD); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_CAP_IN_HAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_METAL_SHOCK); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_TELEPORTING); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_UNKNOWN_08); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_UNKNOWN_13); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_ACTION_SOUND_PLAYED); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_MARIO_SOUND_PLAYED); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_UNKNOWN_18); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_PUNCHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_KICKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_TRIPPING); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_UNKNOWN_25); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_UNKNOWN_30); \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_UNKNOWN_31); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, MARIO_CAP_FLAGS); \

#define PYTHON_MARIO_ADD_ACTION_CONSTANTS(module) \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_ID_MASK); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_MASK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_STATIONARY); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_MOVING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_AIRBORNE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_SUBMERGED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_CUTSCENE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_AUTOMATIC); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUP_OBJECT); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_STATIONARY); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_MOVING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_AIR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_INTANGIBLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_SWIMMING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_METAL_WATER); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_SHORT_HITBOX); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_RIDING_SHELL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_INVULNERABLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_BUTT_OR_STOMACH_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_DIVING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_ON_POLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_HANGING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_IDLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_ATTACKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_CONTROL_JUMP_HEIGHT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_ALLOW_FIRST_PERSON); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_PAUSE_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_SWIMMING_OR_FLYING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_WATER_OR_TEXT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLAG_THROWING); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_UNINITIALIZED); \
    \
    /* group 0x000: stationary actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_IDLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_START_SLEEPING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SLEEPING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WAKING_UP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PANTING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_PANTING_UNUSED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_IDLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_HEAVY_IDLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STANDING_AGAINST_WALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_COUGHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SHIVERING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_IN_QUICKSAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CROUCHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_START_CROUCHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STOP_CROUCHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_START_CRAWLING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STOP_CRAWLING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SLIDE_KICK_SLIDE_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SHOCKWAVE_BOUNCE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FIRST_PERSON); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKFLIP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_JUMP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DOUBLE_JUMP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FREEFALL_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SIDE_FLIP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_JUMP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_FREEFALL_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_AIR_THROW_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TWIRL_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LAVA_BOOST_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TRIPLE_JUMP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LONG_JUMP_LAND_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUND_POUND_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BRAKING_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BUTT_SLIDE_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_BUTT_SLIDE_STOP); \
    \
    /* group 0x040: moving (ground) actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WALKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_WALKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TURNING_AROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FINISH_TURNING_AROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BRAKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_RIDING_SHELL_GROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_HEAVY_WALKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CRAWLING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BURNING_GROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DECELERATING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_DECELERATING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BEGIN_SLIDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_BEGIN_SLIDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BUTT_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STOMACH_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_BUTT_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_STOMACH_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DIVE_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_MOVE_PUNCHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CROUCH_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SLIDE_KICK_SLIDE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HARD_BACKWARD_GROUND_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HARD_FORWARD_GROUND_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKWARD_GROUND_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FORWARD_GROUND_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SOFT_BACKWARD_GROUND_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SOFT_FORWARD_GROUND_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUND_BONK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DEATH_EXIT_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FREEFALL_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DOUBLE_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SIDE_FLIP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_FREEFALL_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_QUICKSAND_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_QUICKSAND_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TRIPLE_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LONG_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKFLIP_LAND); \
    \
    /* group 0x080: airborne actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DOUBLE_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TRIPLE_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKFLIP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STEEP_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WALL_KICK_AIR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SIDE_FLIP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LONG_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DIVE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FREEFALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TOP_OF_POLE_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BUTT_SLIDE_AIR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLYING_TRIPLE_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SHOT_FROM_CANNON); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLYING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_RIDING_SHELL_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_RIDING_SHELL_FALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_VERTICAL_WIND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_FREEFALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_BUTT_SLIDE_AIR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_WATER_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TWIRLING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FORWARD_ROLLOUT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_AIR_HIT_WALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_RIDING_HOOT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GROUND_POUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SLIDE_KICK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_AIR_THROW); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_JUMP_KICK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKWARD_ROLLOUT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CRAZY_BOX_BOUNCE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPECIAL_TRIPLE_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKWARD_AIR_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FORWARD_AIR_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HARD_FORWARD_AIR_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HARD_BACKWARD_AIR_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BURNING_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BURNING_FALL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SOFT_BONK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LAVA_BOOST); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GETTING_BLOWN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_THROWN_FORWARD); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_THROWN_BACKWARD); \
    \
    /* group 0x0C0: submerged actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_IDLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_WATER_IDLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_ACTION_END); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_WATER_ACTION_END); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DROWNING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BACKWARD_WATER_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FORWARD_WATER_KB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_DEATH); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_SHOCKED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BREASTSTROKE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SWIMMING_END); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FLUTTER_KICK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_BREASTSTROKE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_SWIMMING_END); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_FLUTTER_KICK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_SHELL_SWIMMING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_THROW); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_PUNCH); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WATER_PLUNGE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CAUGHT_IN_WHIRLPOOL); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_METAL_WATER_STANDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_METAL_WATER_STANDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_METAL_WATER_WALKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_METAL_WATER_WALKING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_METAL_WATER_FALLING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_METAL_WATER_FALLING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_METAL_WATER_FALL_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_METAL_WATER_FALL_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_METAL_WATER_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_METAL_WATER_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_METAL_WATER_JUMP_LAND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLD_METAL_WATER_JUMP_LAND); \
    \
    /* group 0x100: cutscene actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DISAPPEARED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_INTRO_CUTSCENE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STAR_DANCE_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STAR_DANCE_WATER); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FALL_AFTER_STAR_GRAB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_READING_AUTOMATIC_DIALOG); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_READING_NPC_DIALOG); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STAR_DANCE_NO_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_READING_SIGN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_JUMBO_STAR_CUTSCENE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WAITING_FOR_DIALOG); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DEBUG_FREE_MOVE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STANDING_DEATH); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_QUICKSAND_DEATH); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_ELECTROCUTION); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SUFFOCATION); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DEATH_ON_STOMACH); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DEATH_ON_BACK); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_EATEN_BY_BUBBA); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_END_PEACH_CUTSCENE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CREDITS_CUTSCENE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_END_WAVING_CUTSCENE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PULLING_DOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PUSHING_DOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_WARP_DOOR_SPAWN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_EMERGE_FROM_PIPE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPAWN_SPIN_AIRBORNE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPAWN_SPIN_LANDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_EXIT_AIRBORNE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_EXIT_LAND_SAVE_DIALOG); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DEATH_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_UNUSED_DEATH_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FALLING_DEATH_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPECIAL_EXIT_AIRBORNE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPECIAL_DEATH_EXIT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FALLING_EXIT_AIRBORNE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_UNLOCKING_KEY_DOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_UNLOCKING_STAR_DOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_ENTERING_STAR_DOOR); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPAWN_NO_SPIN_AIRBORNE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SPAWN_NO_SPIN_LANDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BBH_ENTER_JUMP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BBH_ENTER_SPIN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TELEPORT_FADE_OUT); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TELEPORT_FADE_IN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SHOCKED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_SQUISHED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HEAD_STUCK_IN_GROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_BUTT_STUCK_IN_GROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_FEET_STUCK_IN_GROUND); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PUTTING_ON_CAP); \
    \
    /* group 0x140: "automatic" actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLDING_POLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GRAB_POLE_SLOW); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GRAB_POLE_FAST); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_CLIMBING_POLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TOP_OF_POLE_TRANSITION); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TOP_OF_POLE); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_START_HANGING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HANGING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HANG_MOVING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LEDGE_GRAB); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LEDGE_CLIMB_SLOW_1); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LEDGE_CLIMB_SLOW_2); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LEDGE_CLIMB_DOWN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_LEDGE_CLIMB_FAST); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_GRABBED); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_IN_CANNON); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_TORNADO_TWIRLING); \
    \
    /* group 0x180: object actions */ \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PUNCHING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PICKING_UP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_DIVE_PICKING_UP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_STOMACH_SLIDE_STOP); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PLACING_DOWN); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_THROWING); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HEAVY_THROW); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_PICKING_UP_BOWSER); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_HOLDING_BOWSER); \
    PYTHON_MARIO_ADD_CONSTANT(module, ACT_RELEASING_BOWSER); \

#define PYTHON_MARIO_ADD_INTERACTION_CONSTANTS(module) \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_HOOT_GRABBED_BY_MARIO); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_UNK1); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_UNK2); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_DROP_OBJECT); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_UNK4); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_UNK5); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_UNK6); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_MARIO_UNK7); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_GRABBED_MARIO); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_ATTACKED_MARIO); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_WAS_ATTACKED); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_INTERACTED); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_TRAP_TURN); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_HIT_MINE); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_STOP_RIDING); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_STATUS_TOUCHED_BOB_OMB); \
    \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_GROUND_POUND_OR_TWIRL); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_PUNCH); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_KICK); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_TRIP); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_SLIDE_KICK); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_FAST_ATTACK_OR_SHELL); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_HIT_FROM_ABOVE); \
    PYTHON_MARIO_ADD_CONSTANT(module, INT_HIT_FROM_BELOW); \
    \
    PYTHON_MARIO_ADD_COMPOUND_CONSTANT(module, INT_ATTACK_NOT_FROM_BELOW, \
        (INT_GROUND_POUND_OR_TWIRL | INT_PUNCH | INT_KICK | INT_TRIP | INT_SLIDE_KICK \
        | INT_FAST_ATTACK_OR_SHELL | INT_HIT_FROM_ABOVE)); \
    \
    PYTHON_MARIO_ADD_COMPOUND_CONSTANT(module, INT_ANY_ATTACK, \
        (INT_GROUND_POUND_OR_TWIRL | INT_PUNCH | INT_KICK | INT_TRIP | INT_SLIDE_KICK \
        | INT_FAST_ATTACK_OR_SHELL | INT_HIT_FROM_ABOVE | INT_HIT_FROM_BELOW)); \
    \
    PYTHON_MARIO_ADD_COMPOUND_CONSTANT(module, INT_ATTACK_NOT_WEAK_FROM_ABOVE, \
        (INT_GROUND_POUND_OR_TWIRL | INT_PUNCH | INT_KICK | INT_TRIP | INT_HIT_FROM_BELOW));

struct _PyObjectClass;

void python_init_mario();
PyObject* PyInit_mario(void);
u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method);

#endif /* MARIO_PYTHON_H */
