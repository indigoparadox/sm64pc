
import logging

import mario
import objects

logger = logging.getLogger( '' )

def mario_init():
    logging.basicConfig( level=logging.INFO )
    logger.info( 'logger active' )

def spawn_yellow_coin( parent_obj ):

    coin = objects.Object(
        0, parent_obj,
        objects.MODEL_YELLOW_COIN,
        objects.bhvMovingYellowCoin )
    coin.copy_pos_and_angle( parent_obj )
    coin.set_forward_vel( mario.random_float() * 20 )
    coin.set_vel_y( mario.random_float() * 40 + 20 )
    coin.set_move_angle_yaw( mario.random_ushort() )

def set_mario_action_moving( mario_state, action, action_arg ):

    floor_class = mario_state.get_floor_class()
    forward_vel = mario_state.get_forward_vel()
    mag = 8.0

    logger.debug( "moving" )

    if mario.ACT_WALKING == action:
        if floor_class != mario.SURFACE_CLASS_VERY_SLIPPERY:
            if 0.0 <= forward_vel and forward_vel < mag:
                mario_state.set_forward_vel( mag )

        mario_state.mario_object.set_mario_walking_pitch( 0 )

    elif mario.ACT_HOLD_WALKING == action:
        if 0.0 <= forward_vel and forward_vel < mag / 2.0:
            mario_state.set_forward_vel( mag / 2.0 )

    elif mario.ACT_BEGIN_SLIDING == action:
        # Fun fact: if this doesn't get a chance to run and change the
        # "BEGIN_SLIDING" action into a butt slide or a stomach slide,
        # then the system will hang in a loop later in
        # mario_execute_action().
        if (mario_state.facing_downhill(0)):
            action = mario.ACT_BUTT_SLIDE
        else:
            action = mario.ACT_STOMACH_SLIDE

    elif mario.ACT_HOLD_BEGIN_SLIDING == action:
        if mario_state.facing_downhill(0):
            action = mario.ACT_HOLD_BUTT_SLIDE
        else:
            action = mario.ACT_HOLD_STOMACH_SLIDE

    return action

def set_mario_action( mario_state, action, arg ):
   
    logger.debug( "%lu vs %lu", (mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING), action )

    # DEBUG
    if action & mario.ACT_FLAG_ATTACKING:
        spawn_yellow_coin( mario_state.mario_object )
    # END DEBUG

    # Filter based on action group.
    if (mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING):
        logger.debug( "moving" )
        action = set_mario_action_moving( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_AIRBORNE):
        logger.debug( "airborne" )
        action = mario.set_mario_action_airborne( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_SUBMERGED):
        logger.debug( "submerged" )
        action = mario.set_mario_action_submerged( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_CUTSCENE):
        logger.debug( "cutscene" )
        action = mario.set_mario_action_cutscene( mario_state, action, arg )

    logger.debug( "setting action flags..." )

    # Set action flags.
    mario_state.unset_flag( mario.MARIO_ACTION_SOUND_PLAYED | mario.MARIO_MARIO_SOUND_PLAYED )
    if not (mario_state.get_action() & mario.ACT_FLAG_AIR):
        mario_state.unset_flag( mario.MARIO_UNKNOWN_18 )

    logger.debug( "intializing action info..." )

    # Initialize the action information.
    mario_state.set_prev_action( mario_state.get_action() )
    mario_state.set_action( action )
    mario_state.set_action_arg( arg )
    mario_state.set_action_state( 0 )
    mario_state.set_action_timer( 0 )

    return 1
