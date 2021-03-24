
import logging

import mario
import objects
import levels

logger = logging.getLogger( '' )

def mario_init():
    logging.basicConfig( level=logging.DEBUG )
    logger.info( 'logger active' )

def spawn_object( parent_obj, model, behavior ):
    obj = objects.Object( parent_obj, model, behavior )
    obj.copy_pos_and_angle( parent_obj )
    return obj

def spawn_yellow_coins( parent_obj, count ):

    for i in range( count ):
        coin = spawn_object( parent_obj,
            objects.MODEL_YELLOW_COIN,
            objects.bhvMovingYellowCoin )
        coin.set_forward_vel( mario.random_float() * 20 )
        coin.set_vel_y( mario.random_float() * 40 + 20 )
        coin.set_move_angle_yaw( mario.random_ushort() )

def set_mario_action_moving( mario_state, action, action_arg ):

    floor_class = mario_state.get_floor_class()
    forward_vel = mario_state.get_forward_vel()
    mag = min( [mario_state.get_intended_mag(), 8.0] )

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

def set_mario_action_airborne( mario_state, action, action_arg ):
    forward_vel = 0
    mario_forward_vel = mario_state.get_forward_vel()

    if mario_state.get_squish_timer() != 0 or mario_state.get_quicksand_depth() >= 1.0:
        if action == mario.ACT_DOUBLE_JUMP or action == mario.ACT_TWIRLING:
            action = mario.ACT_JUMP

    if mario.ACT_DOUBLE_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 52.0, 0.25 )
        mario_state.set_forward_vel( mario_state.get_forward_vel() * 0.8 )

    elif mario.ACT_BACKFLIP == action:
        mario_state.set_anim_id( -1 )
        mario_state.set_forward_vel( -16.0 )
        mario_state.set_y_vel_based_on_fspeed( 62.0, 0.0 )

    elif mario.ACT_TRIPLE_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 69.0, 0.0 )
        mario_state.set_forward_vel( mario_state.get_forward_vel() * 0.8 )

    elif mario.ACT_FLYING_TRIPLE_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 82.0, 0.0 )

    elif mario.ACT_WATER_JUMP == action or \
    mario.ACT_HOLD_WATER_JUMP == action:
        if 0 == action_arg:
            mario_state.set_y_vel_based_on_fspeed( 42.0, 0.0 )

    elif mario.ACT_BURNING_JUMP == action:
        mario_state.set_vel( 1, 31.5 )
        mario_state.set_forward_vel( 8.0 )

    elif mario.ACT_RIDING_SHELL_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 42.0, 0.25 )

    elif mario.ACT_JUMP == action or \
    mario.ACT_HOLD_JUMP == action:
        # DEBUG
        #if action & mario.ACT_FLAG_ATTACKING:
        #spawn_yellow_coins( mario_state.mario_object, 6 )
        # END DEBUG
        mario_state.set_anim_id( -1 )
        mario_state.set_y_vel_based_on_fspeed( 42.0, 0.25 )
        mario_state.set_forward_vel( mario_state.get_forward_vel() * 0.8 )

    elif mario.ACT_WALL_KICK_AIR == action or \
    mario.ACT_TOP_OF_POLE_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 62.0, 0.0 )
        if mario_forward_vel < 24.0:
            mario_state.set_forward_vel( 24.0 )
        mario_state.set_wall_kick_timer( 0 )

    elif mario.ACT_SIDE_FLIP == action:
        mario_state.set_y_vel_based_on_fspeed( 62.0, 0.0 )
        mario_state.set_forward_vel( 8.0 )
        mario_state.set_face_angle( 1, mario_state.get_intended_yaw() )

    elif mario.ACT_STEEP_JUMP == action:
        mario_state.set_anim_id( -1 )
        mario_state.set_y_vel_based_on_fspeed( 42.0, 0.25 )
        mario_state.set_face_angle( 0, -0x2000 )

    elif mario.ACT_LAVA_BOOST == action:
        mario_state.set_vel( 1, 84.0 )
        if 0 == action_arg:
            mario_state.set_forward_vel( 0.0 )

    elif mario.ACT_DIVE == action:
        forward_vel = mario_forward_vel + 15.0
        if forward_vel > 48.0:
            forward_vel = 48.0
        mario_state.set_forward_vel_all( forward_vel )

    elif mario.ACT_LONG_JUMP == action:
        mario_state.set_anim_id( -1 )
        mario_state.set_y_vel_based_on_fspeed( 30.0, 0.0 )
        if mario_state.get_forward_vel() > 16.0:
            mario_state.mario_object.set_mario_long_jump_is_slow( 0 )
        else:
            mario_state.mario_object.set_mario_long_jump_is_slow( 1 )

        # (BLJ's) This properly handles long jumps from getting forward speed with
        # too much velocity, but misses backwards longs allowing high negative speeds.
        mario_forward_vel *= 1.5
        if 48.0 < mario_forward_vel:
            mario_state.set_forward_vel( 48.0 )

    elif mario.ACT_SLIDE_KICK == action:
        mario_state.set_vel( 1, 12.0 )
        if 32.0 > mario_state.get_forward_vel():
            mario_state.set_forward_vel( 32.0 )

    elif mario.ACT_JUMP_KICK == action:
        mario_state.set_vel( 1, 20.0 )

    mario_state.set_peak_height( mario_state.get_pos( 1 ) )
    mario_state.set_flag( mario.MARIO_UNKNOWN_08 )

    return action

def set_mario_action_submerged( mario_state, action, action_arg ):

    if mario.ACT_METAL_WATER_JUMP == action or \
    mario.ACT_HOLD_METAL_WATER_JUMP == action:
        mario_state.set_vel( 1, 32.0 )

    return action

def set_mario_action_cutscene( mario_state, action, action_arg ):

    if mario.ACT_EMERGE_FROM_PIPE == action:
        mario_state.set_vel( 1, 52.0 )

    elif mario.ACT_FALL_AFTER_STAR_GRAB == action:
        mario_state.set_forward_vel_all( 0.0 )

    elif mario.ACT_SPAWN_SPIN_AIRBORNE == action:
        mario_state.set_forward_vel_all( 2.0 )

    if mario.ACT_SPECIAL_EXIT_AIRBORNE == action or \
    mario.ACT_SPECIAL_DEATH_EXIT == action:
        mario_state.set_vel( 1, 64.0 )

    return action

def set_mario_action( mario_state, action, arg ):

    logger.debug( "%lu vs %lu",
        (mario.ACT_GROUP_MASK & mario.ACT_GROUP_AIRBORNE),
        (action & mario.ACT_GROUP_MASK) )

    # DEBUG
    #if action & mario.ACT_FLAG_ATTACKING:
    #    levels.initiate_warp( levels.LEVEL_BBH, 0x01, 0x0a, 0 )
    # END DEBUG

    # Filter based on action group.
    if (mario.ACT_GROUP_MASK & mario.ACT_GROUP_MOVING) == (action & mario.ACT_GROUP_MASK):
        logger.debug( "moving" )
        action = set_mario_action_moving( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_AIRBORNE) == (action & mario.ACT_GROUP_MASK):
        logger.debug( "airborne" )
        action = set_mario_action_airborne( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_SUBMERGED) == (action & mario.ACT_GROUP_MASK):
        logger.debug( "submerged" )
        action = set_mario_action_submerged( mario_state, action, arg )
    elif (mario.ACT_GROUP_MASK & mario.ACT_GROUP_CUTSCENE) == (action & mario.ACT_GROUP_MASK):
        logger.debug( "cutscene" )
        action = set_mario_action_cutscene( mario_state, action, arg )

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
