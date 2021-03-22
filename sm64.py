
import mario

def set_mario_action( mario_state, action, arg ):

    print( mario.get_action() )
    print( mario.get_action() )

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
    mario.unset_flag( mario.MARIO_ACTION_SOUND_PLAYED | mario.MARIO_MARIO_SOUND_PLAYED )
    #if not (mario.get_action() & mario.ACT_FLAG_AIR):
    #    mario.unset_flag( mario.MARIO_UNKNOWN_18 )

    print( 2 )

    # Initialize the action information.
    mario.set_prev_action( mario.get_action() )
    mario.set_action( action )
    mario.set_action_arg( arg )
    mario.set_action_state( 0 )
    mario.set_action_timer( 0 )

    print( 3 )

    return 1
