
import mario

def set_mario_action( mario_state, action, arg ):
    # Filter based on action group.
    if mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING:
        action = mario.set_mario_action_moving( mario_state, action, arg )
    elif mario.ACT_GROUP_MASK & mario.ACT_GROUP_AIRBORNE:
        action = mario.set_mario_action_airborne( mario_state, action, arg )
    elif mario.ACT_GROUP_MASK & mario.ACT_GROUP_SUBMERGED:
        action = mario.set_mario_action_submerged( mario_state, action, arg )
    elif mario.ACT_GROUP_MASK & mario.ACT_GROUP_CUTSCENE:
        action = mario.set_mario_action_cutscene( mario_state, action, arg )

    # Set action flags.
    mario_state.flags &= ~(mario.MARIO_ACTION_SOUND_PLAYED | mario.MARIO_MARIO_SOUND_PLAYED)
    if not (mario_state.action & mario.ACT_FLAG_AIR):
        mario_state.flags &= ~mario.MARIO_UNKNOWN_18

    # Initialize the action information.
    mario_state.prevAction = mario_state.action
    mario_state.action = action
    mario_state.actionArg = arg
    mario_state.actionState = 0
    mario_state.actionTimer = 0

    return 1
