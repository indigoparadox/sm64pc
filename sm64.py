
import logging

import mario
import objects
import levels
import dialog

logger = logging.getLogger( '' )

# DEBUG
bhv_test = None
# Keep a reference hanging around globally so it doesn't despawn.
test_fish = None
#ready = False
osd_lines = []
frame_delays = []

def delay_frames( frames, callback ):
    frame_delays.append( (frames, callback) )

def show_osd_line( x, y, text, ttl ):
    osd_lines.append( (x, y, text, ttl) )
# END DEBUG

def mario_init():
    logging.basicConfig( level=logging.DEBUG )
    logger.info( 'logger active' )

    # DEBUG
    global bhv_test
    bhv_test = objects.Behavior()
    bhv_test.bhv_BEGIN( objects.OBJ_LIST_DEFAULT )
    bhv_test.bhv_BEGIN_LOOP()
    bhv_test.bhv_END_LOOP()
    print( bhv_test.script )
    bhv_test.compile()
    # END DEBUG

def spawn_object( parent_obj, model, behavior ):
    obj = objects.Object( parent_obj, model, behavior )
    assert( None != obj )
    obj.copy_pos_and_angle( parent_obj )
    return obj

def spawn_yellow_coins( parent_obj, count ):

    for i in range( count ):
        coin = spawn_object( parent_obj,
            objects.MODEL_YELLOW_COIN,
            objects.bhvMovingYellowCoin )
        coin.scale( 0.5, 0.5, 0.5 )
        coin.set_forward_vel( mario.random_float() * 20 )
        coin.set_vel_y( mario.random_float() * 40 + 20 )
        coin.set_move_angle_yaw( mario.random_ushort() )

def dialog_render_frame():
    # DEBUG
    dead_delays = []
    global frame_delays
    for idx in range( len( frame_delays ) ):
        if 0 < frame_delays[idx][0]:
            frame_delays[idx] = (frame_delays[idx][0] - 1, frame_delays[idx][1])
        else:
            frame_delays[idx][1]()
            dead_delays.append( frame_delays[idx] )

    for delay in dead_delays:
        frame_delays.remove( delay )
    
    #global ready
    #if ready:
    dead_lines = []
    for idx in range( len( osd_lines ) ):
        dialog.print_colorful_text(
            osd_lines[idx][0], osd_lines[idx][1], osd_lines[idx][2].encode( 'utf-8' ) )
        if 0 < osd_lines[idx][3]:
            osd_lines[idx] = \
                (osd_lines[idx][0], osd_lines[idx][1], osd_lines[idx][2], osd_lines[idx][3] - 1)
        else:
            dead_lines.append( osd_lines[idx] )

    for line in dead_lines:
        osd_lines.remove( line )
    # END DEBUG

def determine_interaction( mario_state, obj ):
    interaction = 0
    action = mario_state.get_action()

    if action & mario.ACT_FLAG_ATTACKING:
        if action == mario.ACT_PUNCHING or \
        action == mario.ACT_MOVE_PUNCHING or \
        action == mario.ACT_JUMP_KICK:
            yaw_to_object = mario_state.get_angle_to_object(
                mario_state, obj ) - mario_state.get_face_angle( 1 )

            if mario_state.get_flags() & mario.MARIO_PUNCHING:
                # 120 degrees total, or 60 each way
                if -0x2AAA <= yaw_to_object and yaw_to_object <= 0x2AAA:
                    interaction = mario.INT_PUNCH

            if mario_state.get_flags() & mario.MARIO_KICKING:
                # 120 degrees total, or 60 each way
                if -0x2AAA <= yaw_to_object and yaw_to_object <= 0x2AAA:
                    interaction = mario.INT_KICK

            if mario_state.get_flags() & mario.MARIO_TRIPPING:
                # 180 degrees total, or 90 each way
                if -0x4000 <= yaw_to_object and yaw_to_object <= 0x4000:
                    interaction = mario.INT_TRIP

        elif action == mario.ACT_GROUND_POUND or \
        action == mario.ACT_TWIRLING:
            if mario_state.get_vel( 1 ) < 0.0:
                # DEBUG
                show_osd_line( 20, 20, "- BOOM -", 120 )
                # END DEBUG
                interaction = mario.INT_GROUND_POUND_OR_TWIRL

        elif action == mario.ACT_GROUND_POUND_LAND or \
        action == mario.ACT_TWIRL_LAND:
            # Neither ground pounding nor twirling change Mario's vertical speed on landing.,
            # so the speed check is nearly always true (perhaps not if you land while going upwards?)
            # Additionally, actionState it set on each first thing in their action, so this is
            # only true prior to the very first frame (i.e. active 1 frame prior to it run).
            if mario_state.get_vel( 1 ) < 0.0 and \
            mario_state.get_action_state() == 0:
                interaction = mario.INT_GROUND_POUND_OR_TWIRL

        elif action == mario.ACT_SLIDE_KICK or \
        action == mario.ACT_SLIDE_KICK_SLIDE:
            interaction = mario.INT_SLIDE_KICK
        elif action & mario.ACT_FLAG_RIDING_SHELL:
            interaction = mario.INT_FAST_ATTACK_OR_SHELL
        elif mario_state.get_forward_vel() <= -26.0 or \
        26.0 <= mario_state.get_forward_vel():
            interaction = mario.INT_FAST_ATTACK_OR_SHELL

    # Prior to this, the interaction type could be overwritten. This requires, however,
    # that the interaction not be set prior. This specifically overrides turning a ground
    # pound into just a bounce.
    if interaction == 0 and action & mario.ACT_FLAG_AIR:
        if mario_state.get_vel( 1 ) < 0.0:
            if mario_state.get_pos( 1 ) > obj.get_pos_y():
                interaction = mario.INT_HIT_FROM_ABOVE

        else:
            if mario_state.get_pos( 1 ) < obj.get_pos_y():
                interaction = mario.INT_HIT_FROM_BELOW

    return interaction

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
        # DEBUG
        #if action & mario.ACT_FLAG_ATTACKING:
        show_osd_line( 20, 20, "+ AIR TIME +", 120 )
        spawn_yellow_coins( mario_state.mario_object, 6 )
        # END DEBUG

    elif mario.ACT_TRIPLE_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 69.0, 0.0 )
        mario_state.set_forward_vel( mario_state.get_forward_vel() * 0.8 )
        # DEBUG
        #if action & mario.ACT_FLAG_ATTACKING:
        show_osd_line( 20, 20, "+ AIR TIME +", 120 )
        spawn_yellow_coins( mario_state.mario_object, 6 )
        # END DEBUG

    elif mario.ACT_FLYING_TRIPLE_JUMP == action:
        mario_state.set_y_vel_based_on_fspeed( 82.0, 0.0 )
        # DEBUG
        #if action & mario.ACT_FLAG_ATTACKING:
        show_osd_line( 20, 20, "+ AIR TIME +", 120 )
        spawn_yellow_coins( mario_state.mario_object, 6 )
        # END DEBUG

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
    #global ready
    #ready = True
    global test_fish
    global bhv_test
    if action & mario.ACT_FLAG_ATTACKING:
        # TODO: Why doesn't Mario scale?
        mario_state.mario_object.scale( 2.0, 2.0, 2.0 )
        delay_frames( 20, lambda: mario_state.mario_object.scale( 1, 1, 1 ) )
    #    spawn_yellow_coins(mario_state.mario_object, 6)
    #    test_fish = spawn_object( mario_state.mario_object, objects.MODEL_NONE, bhv_test )
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
