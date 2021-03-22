
import mario

def set_mario_action_moving( m, action, action_arg ):
    floorClass = m.get_floor_class()
    forwardVel = m.get_forward_vel()
    #mag = min(m.get_intended_mag(), 8.0)
    mag = 8.0

    print( "moving" )

    if mario.ACT_WALKING == action:
        if floorClass != mario.SURFACE_CLASS_VERY_SLIPPERY:
            if 0.0 <= forwardVel and forwardVel < mag:
                mario.state.set_forward_vel( mag )

        #mario.->marioObj->oMarioWalkingPitch = 0

    elif mario.ACT_HOLD_WALKING == action:
        if 0.0 <= forwardVel and forwardVel < mag / 2.0:
            mario.state.set_forward_vel( mag / 2.0 )

    elif mario.ACT_BEGIN_SLIDING == action:
        if (m.facing_downhill(0)):
            action = mario.ACT_BUTT_SLIDE
        else:
            action = mario.ACT_STOMACH_SLIDE

    elif mario.ACT_HOLD_BEGIN_SLIDING == action:
        if m.facing_downhill(0):
            action = mario.ACT_HOLD_BUTT_SLIDE
        else:
            action = mario.ACT_HOLD_STOMACH_SLIDE

    return action

def set_mario_action( mario_state, action, arg ):
    
    print( "{} vs {}".format( (mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING), action ) )

    # Filter based on action group.
    if (mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING):
        print( "moving" )
        action = set_mario_action_moving( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_AIRBORNE):
        print( "airborne" )
        action = mario.set_mario_action_airborne( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_SUBMERGED):
        print( "submerged" )
        action = mario.set_mario_action_submerged( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_CUTSCENE):
        print( "cutscene" )
        action = mario.set_mario_action_cutscene( mario_state, action, arg )

    # Set action flags.
    mario_state.unset_flag( mario.MARIO_ACTION_SOUND_PLAYED | mario.MARIO_MARIO_SOUND_PLAYED )
    if not (mario.state.get_action() & mario.ACT_FLAG_AIR):
        mario.state.unset_flag( mario.MARIO_UNKNOWN_18 )

    # Initialize the action information.
    mario_state.set_prev_action( mario.state.get_action() )
    mario_state.set_action( action )
    mario_state.set_action_arg( arg )
    mario_state.set_action_state( 0 )
    mario_state.set_action_timer( 0 )

    return 1
