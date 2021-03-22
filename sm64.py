
import mario

'''
def set_mario_action_moving( action, action_arg ):
    floorClass = mario_get_floor_class(m)
    forwardVel = mario.state.get_forward_vel()
    mag = min(mario.state.get_intended_mag(), 8.0)

    if ACT_WALKING == action:
        if floorClass != SURFACE_CLASS_VERY_SLIPPERY:
            if 0.0 <= forwardVel and forwardVel < mag:
                mario.state.set_forward_vel( mag )

        #mario.->marioObj->oMarioWalkingPitch = 0

    elif ACT_HOLD_WALKING == action:
        if 0.0 <= forwardVel and forwardVel < mag / 2.0:
            mario.state.set_forward_vel( mag / 2.0 )

    elif ACT_BEGIN_SLIDING == action:
        if (mario_facing_downhill(m, FALSE)) {
            action = ACT_BUTT_SLIDE;
        } else {
            action = ACT_STOMACH_SLIDE;
        }

    elif ACT_HOLD_BEGIN_SLIDING == action:
        if mario_facing_downhill(m, FALSE):
            action = ACT_HOLD_BUTT_SLIDE
        else:
            action = ACT_HOLD_STOMACH_SLIDE

    return action
'''

def set_mario_action( mario_state, action, arg ):

    print( mario.state )

    # Filter based on action group.
    '''if (mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING) == action:
        action = mario.set_mario_action_moving( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_AIRBORNE) == action:
        action = mario.set_mario_action_airborne( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_SUBMERGED) == action:
        action = mario.set_mario_action_submerged( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_CUTSCENE) == action:
        action = mario.set_mario_action_cutscene( mario_state, action, arg )'''

    print( 1 )

    # Set action flags.
    mario.state.unset_flag( mario.MARIO_ACTION_SOUND_PLAYED | mario.MARIO_MARIO_SOUND_PLAYED )
    print( 1.5 )
    if not (mario.state.get_action() & mario.ACT_FLAG_AIR):
        mario.state.unset_flag( mario.MARIO_UNKNOWN_18 )

    print( 2 )

    # Initialize the action information.
    mario.state.set_prev_action( mario.state.get_action() )
    mario.state.set_action( action )
    mario.state.set_action_arg( arg )
    mario.state.set_action_state( 0 )
    mario.state.set_action_timer( 0 )

    print( 3 )

    return 1
