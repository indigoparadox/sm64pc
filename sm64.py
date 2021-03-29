
import logging
import collections

import mario
import objects
import levels
import dialog
import save_file

logger = logging.getLogger( '' )

InteractionHandler = collections.namedtuple('InteractionHandler', ['interact_type', 'handler'])

forward_knockback_actions = [
    [mario.ACT_SOFT_FORWARD_GROUND_KB, mario.ACT_FORWARD_GROUND_KB, mario.ACT_HARD_FORWARD_GROUND_KB],
    [mario.ACT_FORWARD_AIR_KB,         mario.ACT_FORWARD_AIR_KB,    mario.ACT_HARD_FORWARD_AIR_KB],
    [mario.ACT_FORWARD_WATER_KB,       mario.ACT_FORWARD_WATER_KB,  mario.ACT_FORWARD_WATER_KB]]

backward_knockback_actions = [
    [mario.ACT_SOFT_BACKWARD_GROUND_KB, mario.ACT_BACKWARD_GROUND_KB, mario.ACT_HARD_BACKWARD_GROUND_KB],
    [mario.ACT_BACKWARD_AIR_KB,         mario.ACT_BACKWARD_AIR_KB,    mario.ACT_HARD_BACKWARD_AIR_KB],
    [mario.ACT_BACKWARD_WATER_KB,       mario.ACT_BACKWARD_WATER_KB,  mario.ACT_BACKWARD_WATER_KB]]

displaying_door_text = False
invulnerable = False
delay_invinc_timer = False

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
    logging.basicConfig( level= logging.DEBUG )
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

def should_push_or_pull_door( mario_state, obj ):
    dx = obj.get_pos_x - mario_state.get_pos( 0 )
    dz = obj.get_pos_z - mario_state.get_pos( 2 )

    yaw = obj.get_move_angle_yaw() - mario.atan2s( dz, dx )

    return 0x00000001 if (yaw >= -0x4000 and yaw <= 0x4000) else 0x00000002

def bounce_back_from_attack( mario_state, interaction ):
    if interaction & (INT_PUNCH | INT_KICK | INT_TRIP):
        if mario_state.get_action() == ACT_PUNCHING:
            mario_state.set_action( ACT_MOVE_PUNCHING )

        if mario_state.get_action() & ACT_FLAG_AIR:
            mario_state.set_forward_vel_all( -16.0 )
        else:
            mario_state.set_forward_vel_all( -48.0 )

        # TODO: Camera stuff.
        #set_camera_shake_from_hit(SHAKE_ATTACK);
        mario_state.set_particle_flags( PARTICLE_TRIANGLE )

    # TODO: Sound stuff.
    #if (interaction & (INT_PUNCH | INT_KICK | INT_TRIP | INT_FAST_ATTACK_OR_SHELL)) {
    #    play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);

def get_door_save_file_flag( door ):

    save_file_flag = 0
    required_num_stars = get.get_beh_params() >> 24

    is_ccm_door = door.get_pos_x() < 0.0
    is_pss_door = door.get_pos_y() > 500.0

    if 1 == required_num_stars:
        if is_pss_door:
            save_file_flag = save_file.SAVE_FLAG_UNLOCKED_PSS_DOOR
        else:
            save_file_flag = save_file.SAVE_FLAG_UNLOCKED_WF_DOOR

    elif 3 == required_num_stars:
        if is_ccm_door:
            save_file_flag = save_file.SAVE_FLAG_UNLOCKED_CCM_DOOR
        else:
            save_file_flag = save_file.SAVE_FLAG_UNLOCKED_JRB_DOOR

    elif 8 == required_num_stars:
        save_file_flag = save_file.SAVE_FLAG_UNLOCKED_BITDW_DOOR
    
    elif 30 == required_num_stars:
        save_file_flag = save_file.SAVE_FLAG_UNLOCKED_BITFS_DOOR

    elif 50 == required_num_stars:
        save_file_flag = save_file.SAVE_FLAG_UNLOCKED_50_STAR_DOOR

    return save_file_flag

def check_object_grab_mario( mario_state, interact_type, obj ):

    air_invul_attacking = \
        ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ATTACKING
    if (not mario_state.get_action() & air_invul_attacking or \
        not invulnerable) and \
    obj.get_interaction_subtype() & INT_SUBTYPE_GRABS_MARIO:

        if object_facing_mario( mario_state, obj, 0x2AAA ):
            mario_state.stop_riding_and_holding()
            obj.set_interact_status(
                mario.INT_STATUS_INTERACTED | mario.INT_STATUS_GRABBED_MARIO )

            mario_state.set_face_angle( 1, obj.get_move_angle_yaw() )
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )

            # TODO: Sound stuff.
            #mario_state.update_sound_and_camera()
            #play_sound(SOUND_MARIO_OOOF, mario_state.marioObj->header.gfx.cameraToObject);
            # TODO: Rumble stuff.
            # queue_rumble_data\(5, 80\);
            return set_mario_action( mario_state, mario.ACT_GRABBED, 0 )

    mario_state.push_out_of_object( obj, -5.0 )

    return False

def object_facing_mario( mario_state, obj, angle_range ):
    dx = mario_state.get_pos( 0 ) - obj.get_pos_x()
    dz = mario_state.get_pos( 2 ) - obj.get_pos_z()

    angle_to_mario = mario.atan2s( dz, dx )
    angle = angle_to_mario - o.get_move_angle_yaw()

    if (-1 * angle_range) <= angle and angle <= angle_range:
        return True

    return False

def take_damage_from_interact_object( mario_state ):
    shake = 0
    damage = mario_state.get_interact_obj().get_damage_or_coin_value()

    # TODO: Camera stuff.
    #if damage >= 4:
    #    shake = SHAKE_LARGE_DAMAGE;
    #} else if (damage >= 2) {
    #    shake = SHAKE_MED_DAMAGE;
    #} else {
    #    shake = SHAKE_SMALL_DAMAGE;

    if not mario_state.get_flags() & MARIO_CAP_ON_HEAD:
        damage += (damage + 1) / 2

    if mario_state.get_flags() & mario.MARIO_METAL_CAP:
        damage = 0

    mario_state.set_hurt_counter( mario_state.get_hurt_counter() + 4 * damage )

    # TODO: Rumble stuff.
    #queue_rumble_data(5, 80);
    # TODO: Camera stuff.
    #set_camera_shake_from_hit(shake);

    return damage

def take_damage_and_knock_back( mario_state,  obj ):
    damage = 0

    if not invulnerable and \
    not mario_state.get_flags() & mario.MARIO_VANISH_CAP and \
    not o.get_interaction_subtype() & mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        obj.set_interact_status(
            mario.INT_STATUS_INTERACTED | mario.INT_STATUS_ATTACKED_MARIO )
        mario_state.set_interact_obj( obj )

        damage = take_damage_from_interact_object( mario_state )

        if obj.get_interaction_subtype() & mario.INT_SUBTYPE_BIG_KNOCKBACK:
            mario_state.set_forward_vel( 40.0 )

        if obj.get_damage_or_coin_value() > 0:
            pass
            # TODO: Sound stuff.
            # play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);

        # TODO: Sound stuff.
        #update_mario_sound_and_camera(m);
        return drop_and_set_mario_action(
            mario_state, determine_knockback_action(
                mario_state, obj.get_damage_or_coin_value() ), damage )

    return False

def bounce_off_object( mario_state, obj, vel_y ):
    mario_state.set_pos( 1, obj.get_pos_y() + obj.get_hitbox_height() )
    mario_state.set_vel( 1, vel_y )

    mario_state.unset_flag( mario.MARIO_UNKNOWN_08 )

    # TODO: Sound stuff.
    #play_sound(SOUND_ACTION_BOUNCE_OFF_OBJECT, m->marioObj->header.gfx.cameraToObject)

def hit_object_from_below( mario_state, obj ):
    mario_state.set_vel( 1, 0.0 )
    # TODO: Camera stuff.
    #set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);


def attack_object( obj, interaction ):
    attack_type = 0

    if mario.INT_GROUND_POUND_OR_TWIRL:
            attack_type = ATTACK_GROUND_POUND_OR_TWIRL

    elif mario.INT_PUNCH:
            attack_type = ATTACK_PUNCH

    elif mario.INT_KICK == interaction or \
    mario.INT_TRIP == interaction:
            attack_type = ATTACK_KICK_OR_TRIP

    elif mario.INT_SLIDE_KICK == interaction or\
    mario.INT_FAST_ATTACK_OR_SHELL == interaction:
        attack_type = ATTACK_FAST_ATTACK

    elif mario.INT_HIT_FROM_ABOVE == interaction:
        attack_type = ATTACK_FROM_ABOVE

    elif mario.INT_HIT_FROM_BELOW == interaction:
        attack_type = ATTACK_FROM_BELOW

    obj.set_interact_status(
        attack_type + (INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED) )

    return attack_type

#region spawn

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

#endregion

#region dialog

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

#endregion

#region interaction

def interact_coin( mario_state, interact_type, obj ):
    num_coins = mario_state.get_num_coins()
    coin_val = obj.get_damage_or_coin_value()

    mario_state.set_num_coins( num_coins + coin_val )
    mario_state.set_heal_counter( 4 * coin_val )

    obj.set_interact_status( mario.INT_STATUS_INTERACTED )

    if COURSE_IS_MAIN_COURSE(gCurrCourseNum) and \
    num_counts - coin_val < 100 and \
    num_coints >= 100:
        # bhv_spawn_star_no_level_exit(6):
        coin_star = spawn_object( obj, objectsMODEL_STAR, objects.bhvSpawnedStarNoLevelExit )
        coin_star.set_beh_params( 6 << 24 )
        coin_star.set_interaction_subtype( mario.INT_SUBTYPE_NO_EXIT )
        obj.set_angle( 0, 0, 0 )
   
    # TODO: Rumble stuff.
    #if obj.oDamageOrCoinValue >= 2:
    # queue_rumble_data\(5, 80\);

    return False

def interact_water_ring( mario_state, interact_type, obj ):
    num_coins = mario_state.get_num_coins()
    mario_state.set_heal_counter( 4 * num_coins )
    obj.set_interact_status( mario.INT_STATUS_INTERACTED )
    return False

def interact_star_or_key( mario_state, interact_type, obj ):
    interaction_subtype = obj.get_interaction_subtype()
    star_grab_action = mario.ACT_STAR_DANCE_EXIT
    no_exit = interaction_subtype & mario.INT_SUBTYPE_NO_EXIT
    grand_star = interaction_subtype & mario.INT_SUBTYPE_GRAND_STAR

    if mario_state.health >= 0x100:
        mario_state.stop_riding_and_holding()
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);

        if not no_exit:
            mario_state.set_hurt_counter( 0 )
            mario_state.set_heal_counter( 0 )
            if mario_state.get_cap_timer() > 1:
                mario_state.set_cap_timer( 1 )

        if no_exit:
            star_grab_action = mario.ACT_STAR_DANCE_NO_EXIT

        if mario_state.action &mario.ACT_FLAG_SWIMMING:
            star_grab_action = mario.ACT_STAR_DANCE_WATER

        if mario_state.action &mario.ACT_FLAG_METAL_WATER:
            star_grab_action = mario.ACT_STAR_DANCE_WATER

        if mario_state.action &mario.ACT_FLAG_AIR:
            star_grab_action = mario.ACT_FALL_AFTER_STAR_GRAB

        spawn_object( obj, objects.MODEL_NONE, objects.bhvStarKeyCollectionPuffSpawner )

        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( o )

        star_index = (obj.get_beh_params() >> 24) & 0x1F
        save_file.collect_star_or_key( mario_state.get_num_coins(), star_index )

        mario_state.set_num_stars(
            save_file.get_total_star_count(
                save_file.get_curr_save_file_num() - 1,
                save_file.COURSE_MIN - 1,
                save_file.COURSE_MAX - 1 ) )

        # TODO: Sound stuff.
        #if not no_exit:
        #    drop_queued_background_music();
        #    fadeout_level_music(126);
        #
        #play_sound(SOUND_MENU_STAR_SOUND, mario_state.marioObj->header.gfx.cameraToObject);
        #mario_state.update_sound_and_camera()

        if grand_star:
            return set_mario_action( mario_state, mario.ACT_JUMBO_STAR_CUTSCENE, 0 )

        return set_mario_action( mario_state, starGrabAction, no_exit + 2 * grand_star )

    return False

def interact_bbh_entrance( mario_state, interact_type, obj ):
    action = mario_state.get_action()
    if action != mario.ACT_BBH_ENTER_SPIN and \
    action != mario.ACT_BBH_ENTER_JUMP:
        mario_state.stop_riding_and_holding()

        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        if action & mario.ACT_FLAG_AIR:
            return set_mario_action( mario_state, mario.ACT_BBH_ENTER_SPIN, 0 )

        return set_mario_action( mario_state, mario.ACT_BBH_ENTER_JUMP, 0 )

    return False

def interact_warp( mario_state, interact_type, obj ):

    if obj.get_interaction_subtype() & mario.INT_SUBTYPE_FADING_WARP:
        action = mario_state.get_action()

        if action == mario.ACT_TELEPORT_FADE_IN:
            just_teleported = True

        elif not just_teleported:
            if action == mario.ACT_IDLE or \
            action == mario.ACT_PANTING or \
            action == mario.ACT_STANDING_AGAINST_WALL or \
            action == mario.ACT_CROUCHING:
                mario_state.set_interact_obj( obj )
                mario_state.set_used_obj( obj )

                just_teleported = True

                return set_mario_action( mario_state, mario.ACT_TELEPORT_FADE_OUT, 0 )

    else:
        if mario_state.get_action() != mario.ACT_EMERGE_FROM_PIPE:
            obj.set_interact_status( mario.INT_STATUS_INTERACTED )
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )

            if obj.collisionData == segmented_to_virtual(warp_pipe_seg3_collision_03009AC8):
                pass
                # TODO: Sound stuff.
                #play_sound(SOUND_MENU_ENTER_PIPE, mario_state.marioObj->header.gfx.cameraToObject);
                # TODO: Rumble stuff.
                # queue_rumble_data\(15, 80\);
            else:
                pass
                # TODO: Sound stuff.
                #play_sound(SOUND_MENU_ENTER_HOLE, mario_state.marioObj->header.gfx.cameraToObject);
                # TODO: Rumble stuff.
                # queue_rumble_data\(12, 80\);

            mario_state.stop_riding_object()
            return set_mario_action(
                mario_state,
                mario.ACT_DISAPPEARED,
                (levels.WARP_OP_WARP_OBJECT << 16) + 2 )

    return False

def interact_warp_door( mario_state, interact_type, obj ):

    action_arg = 0
    door_action = 0
    action = mario_state.get_action()
    save_flags = save_file.get_flags()
    warp_door_id = obj.get_beh_params() >> 24

    if action == mario.ACT_WALKING or \
    action == mario.ACT_DECELERATING:
        if warp_door_id == 1 and \
        not save_flags & save_file.SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR:
            if not save_flags & save_file.SAVE_FLAG_HAVE_KEY_2:
                if not displaying_door_text:
                    set_mario_action(
                        mario_state,
                        mario.ACT_READING_AUTOMATIC_DIALOG,
                        dialog.DIALOG_023 \
                            if save_flags & save_file.SAVE_FLAG_HAVE_KEY_1 \
                            else dialog.DIALOG_022 )

                displaying_door_text = True

                return False

            door_action = mario.ACT_UNLOCKING_KEY_DOOR

        if warp_door_id == 2 and \
        not save_flags & save_file.SAVE_FLAG_UNLOCKED_BASEMENT_DOOR:
            if not save_flags & save_file.SAVE_FLAG_HAVE_KEY_1:
                if not displaying_door_text:
                    # Moat door skip was intended confirmed
                    set_mario_action(
                        mario_state,
                        mario.ACT_READING_AUTOMATIC_DIALOG,
                        dialog.DIALOG_023 \
                            if save_flags & save_file.SAVE_FLAG_HAVE_KEY_2 \
                            else dialog.DIALOG_022 )
                displaying_door_text = True

                return False

            door_action = mario.ACT_UNLOCKING_KEY_DOOR

        if mario_state.action == mario.ACT_WALKING or \
        mario_state.action == mario.ACT_DECELERATING:
            action_arg = should_push_or_pull_door( mario_state, obj ) + 0x00000004

            if door_action == 0:
                if action_arg & 0x00000001:
                    door_action = mario.ACT_PULLING_DOOR
                else:
                    door_action = mario.ACT_PUSHING_DOOR

            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )
            return set_mario_action( mario_state, door_action, action_arg )

    return False

def interact_door( mario_state, interact_type, obj ):
    required_num_stars = obj.get_beh_params() >> 24
    num_stars = save_file_get_total_star_count(
        save_file.get_curr_save_file_num() - 1,
        levels.COURSE_MIN - 1,
        levels.COURSE_MAX - 1 )

    if mario_state.action == mario.ACT_WALKING or mario_state.action == mario.ACT_DECELERATING:
        if num_stars >= required_num_stars:
            action_arg = should_push_or_pull_door( mario_state, obj )
            enter_door_action = 0
            door_save_file_flag = 0

            if action_arg & 0x00000001:
                enter_door_action = mario.ACT_PULLING_DOOR
            else:
                enter_door_action = mario.ACT_PUSHING_DOOR

            door_save_file_flag = get_door_save_file_flag( obj )
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )

            if obj.get_interaction_subtype() & mario.INT_SUBTYPE_STAR_DOOR:
                enter_door_action = mario.ACT_ENTERING_STAR_DOOR

            if door_save_file_flag != 0 and \
            not save_file.get_flags() & door_save_file_flag:
                enter_door_action = mario.ACT_UNLOCKING_STAR_DOOR

            return set_mario_action(
                mario_state, enter_door_action, action_arg )

        elif not displaying_door_text:
            text = dialog.DIALOG_022 << 16

            if 1 == required_num_stars:
                text = dialog.DIALOG_024 << 16

            elif 3 == required_num_stars:
                text = dialog.DIALOG_025 << 16

            elif 8 == required_num_stars:
                text = dialog.DIALOG_026 << 16

            elif 30 == required_num_stars:
                text = dialog.DIALOG_027 << 16

            elif 50 == required_num_stars:
                text = dialog.DIALOG_028 << 16

            elif 70 == required_num_stars:
                text = dialog.DIALOG_029 << 16

            text += required_num_stars - num_stars

            displaying_door_text = True
            return set_mario_action(
                mario_state,
                mario.ACT_READING_AUTOMATIC_DIALOG,
                text )

    elif mario_state.action == mario.ACT_IDLE and \
    displaying_door_text and \
    required_num_stars == 70:
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )
        return set_mario_action(
            mario_state,
            mario.ACT_ENTERING_STAR_DOOR,
            should_push_or_pull_door( mario_state, obj ) )

    return False

def interact_cannon_base( mario_state, interact_type, obj ):
    if mario_state.action != mario.ACT_IN_CANNON:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )
        return set_mario_action( mario_state, mario.ACT_IN_CANNON, 0 )

    return False

def interact_igloo_barrier( mario_state, interact_type, obj ):
    #! Sets used object without changing action (LOTS of interesting glitches,
    # but unfortunately the igloo barrier is the only object with this interaction
    # type)
    mario_state.set_interact_obj( obj )
    mario_state.set_used_obj( obj )
    mario_state.push_out_of_object( obj, 5.0 )
    return False

def interact_tornado( mario_state, interact_type, obj ):
    mario_obj = mario_state.mario_object

    if mario_state.action != mario.ACT_TORNADO_TWIRLING and mario_state.action != mario.ACT_SQUISHED:
        mario_state.stop_riding_and_holding()
        mario_state.set_forward_vel_all( 0.0 )
        mario_state.update_sound_and_camera()

        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        marioObj.set_mario_tornado_yaw_vel( 0x400 )
        marioObj.set_mario_tornado_pos_y( mario_state.get_pos( 1 ) - obj.get_pos_y() )

        # TODO: Sound stuff.
        #play_sound(SOUND_MARIO_WAAAOOOW, mario_state.marioObj->header.gfx.cameraToObject);
        # TODO: Rumble stuff.
        # queue_rumble_data\(30, 60\);
        
        return set_mario_action(
            mario_state,
            mario.ACT_TORNADO_TWIRLING,
            mario_state.action == mario.ACT_TWIRLING )

    return False

def interact_whirlpool( mario_state, interact_type, obj ):
    mario_obj = mario_state.mario_object

    if mario_state.action != mario.ACT_CAUGHT_IN_WHIRLPOOL:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        mario_state.set_forward_vel( 0.0 )

        mario_obj.set_mario_whirlpool_pos_y( mario_state.get_pos( 1 ) - obj.get_pos_y() )

        # TODO: Sound stuff.
        #play_sound(SOUND_MARIO_WAAAOOOW, mario_state.marioObj->header.gfx.cameraToObject);
        # TODO: Rumble stuff.
        # queue_rumble_data\(30, 60\);
        
        return set_mario_action(
            mario_state,
            mario.ACT_CAUGHT_IN_WHIRLPOOL,
            0 )

    return False

def interact_strong_wind( mario_state, interact_type, obj ):

    if mario_state.action != mario.ACT_GETTING_BLOWN:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        mario_state.set_face_angle( 1, obj.get_move_angle_yaw() + 0x8000 )
        mario_state.set_unkC4( 0.4 )
        mario_state.set_forward_vel( -24.0 )
        mario_state.set_vel( 1, 12.0 )

        # TODO: Sound stuff.
        #play_sound(SOUND_MARIO_WAAAOOOW, mario_state.marioObj->header.gfx.cameraToObject);
        #mario_state.update_sound_and_camera()
        return set_mario_action( mario_state, mario.ACT_GETTING_BLOWN, 0 )

    return False

def interact_flame( mario_state, interact_type, obj ):
    burning_action = mario.ACT_BURNING_JUMP

    if not invulnerable and \
    not mario_state.get_flags() & MARIO_METAL_CAP and \
    not mario_state.get_flags() & MARIO_VANISH_CAP and \
    not obj.get_interaction_subtype() & mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )

        if mario_state.get_action() & \
        (mario.ACT_FLAG_SWIMMING | mario.ACT_FLAG_METAL_WATER) or \
        mario_state.get_water_level() - mario_state.get_pos( 1 ) > 50.0:
            pass
            # TODO: Sound stuff.
            #play_sound(SOUND_GENERAL_FLAME_OUT, mario_state.marioObj->header.gfx.cameraToObject);
        else:
            mario_state.mario_object.set_mario_burn_timer( 0 )
            mario_state.update_sound_and_camera()
            # TODO: Sound stuff.
            #play_sound(SOUND_MARIO_ON_FIRE, mario_state.marioObj->header.gfx.cameraToObject);

            if mario_state.get_action() & mario.ACT_FLAG_AIR and \
            mario_state.get_vel( 1 ) <= 0.0:
                burning_action = mario.ACT_BURNING_FALL

            return drop_and_set_mario_action( mario_state, burning_action, 1 )

    return False

def interact_snufit_bullet( mario_state, interact_type, obj ):

    if not invulnerable and \
    not mario_state.get_flags() & MARIO_VANISH_CAP:
        if mario_state.get_flags() & MARIO_METAL_CAP:
            obj.set_interact_status(
                mario.INT_STATUS_INTERACTED | mario.INT_STATUS_WAS_ATTACKED )
            # TODO: Sound stuff
            #play_sound(SOUND_ACTION_UNKNOWN458, mario_state.marioObj->header.gfx.cameraToObject);

        else:
            obj.set_interact_status(
                mario.INT_STATUS_INTERACTED | mario.INT_STATUS_ATTACKED_MARIO )
            mario_state.set_interact_obj( obj )
            take_damage_from_interact_object( mario_state )

            # TODO: Sound stuff.
            #play_sound(SOUND_MARIO_ATTACKED, mario_state.marioObj->header.gfx.cameraToObject);
            #mario_state.update_sound_and_camera()

            return drop_and_set_mario_action(
                mario_state,
                determine_knockback_action(
                    mario_state, obj.get_damage_or_coin_value() ),
                obj.get_damage_or_coin_value() )

    if not obj.get_interaction_subtype() & mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

def interact_clam_or_bubba( mario_state, interact_type, obj ):

    if obj.get_interaction_subtype() & mario.INT_SUBTYPE_EATS_MARIO:
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        return set_mario_action( mario_state, mario.ACT_EATEN_BY_BUBBA, 0 )

    elif take_damage_and_knock_back( mario_state, obj ):
        return True

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return True

def interact_bully( mario_state, interact_type, obj ):

    interaction = 0

    if mario_state.get_flags() & MARIO_METAL_CAP:
        interaction = mario.INT_FAST_ATTACK_OR_SHELL
    else:
        interaction = determine_interaction( mario_state, obj )

    mario_state.set_interact_obj( obj )

    if interaction & mario.INT_ATTACK_NOT_FROM_BELOW:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        mario_state.push_out_of_object( obj, 5.0 )

        mario_state.set_forward_vel( -16.0 )
        obj.set_move_angle_yaw( mario_state.get_face_angle( 1 ) )
        obj.set_forward_vel( 3392.0 / obj.get_hitbox_radius() )

        attack_object( obj, interaction )
        bounce_back_from_attack( mario_state, interaction )
        
        return True

    elif not invulnerable and \
    not mario_state.get_flags() & MARIO_VANISH_CAP and \
    not (obj.get_nteraction_subtype() & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_invinc_timer( 2 )

        # TODO: Sound stuff.
        #mario_state.update_sound_and_camera()
        #play_sound(SOUND_MARIO_EEUH, mario_state.marioObj->header.gfx.cameraToObject);
        #play_sound(SOUND_OBJ_BULLY_METAL, mario_state.marioObj->header.gfx.cameraToObject);

        mario_state.push_out_of_object( obj, 5.0 )
        drop_and_set_mario_action(
            mario_state, mario_state.bully_knock_back(), 0 )
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        
        return True

    return False

def interact_shock( mario_state, interact_type, obj ):

    action_air = mario.ACT_FLAG_AIR | mario.ACT_FLAG_ON_POLE | mario.ACT_FLAG_HANGING

    if not invulnerable and \
    not mario_state.get_flags() & MARIO_VANISH_CAP and \
    not obj.get_interaction_subtype() & mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        action_arg = (mario_state.get_action() & action_air) == 0

        obj.set_interact_status(
            mario.INT_STATUS_INTERACTED | mario.INT_STATUS_ATTACKED_MARIO )
        mario_state.set_interact_obj( obj )

        take_damage_from_interact_object( mario_state )
        
        # TODO: Sound stuff.
        #play_sound(SOUND_MARIO_ATTACKED, mario_state.marioObj->header.gfx.cameraToObject);
        # TODO: Rumble stuff.
        # queue_rumble_data\(70, 60\);

        if mario_state.get_action() & \
        (mario.ACT_FLAG_SWIMMING | mario.ACT_FLAG_METAL_WATER):
            return drop_and_set_mario_action(
                mario_state, mario.ACT_WATER_SHOCKED, 0 )
        else:
            # TODO: Camera stuff.
            #mario_state.update_sound_and_camera()
            return drop_and_set_mario_action(
                mario_state, mario.ACT_SHOCKED, action_arg )

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

def interact_mr_blizzard( mario_state, interact_type, obj ):
    if take_damage_and_knock_back( mario_state, obj ):
        return True

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

def interact_hit_from_below( mario_state, interact_type, obj ):

    interaction = 0
    if mario_state.get_flags() & MARIO_METAL_CAP:
        interaction = mario.INT_FAST_ATTACK_OR_SHELL
    else:
        interaction = determine_interaction( mario_state, obj )

    if interaction & mario.INT_ANY_ATTACK:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        attack_object( obj, interaction )
        bounce_back_from_attack( mario_state, interaction )

        if interaction & mario.INT_HIT_FROM_BELOW:
            hit_object_from_below( mario_state, obj )

        if interaction & mario.INT_HIT_FROM_ABOVE:
            if obj.get_interaction_subtype() & mario.INT_SUBTYPE_TWIRL_BOUNCE:
                bounce_off_object( mario_state, obj, 80.0 )
                mario_state.reset_pitch()
                # TODO: Sound stuff.
                #play_sound(SOUND_MARIO_TWIRL_BOUNCE, mario_state.marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(
                    mario_state, mario.ACT_TWIRLING, 0 )
            else:
                bounce_off_object( mario_state, obj, 30.0 )

    elif take_damage_and_knock_back( mario_state, obj ):
        return True

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

def interact_bounce_top( mario_state, interact_type, obj ):
    interaction = 0
    if mario_state.get_flags() & MARIO_METAL_CAP:
        interaction = mario.INT_FAST_ATTACK_OR_SHELL
    else:
        interaction = determine_interaction( mario_state, obj )

    if interaction & mario.INT_ATTACK_NOT_FROM_BELOW:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        attack_object( obj, interaction )
        bounce_back_from_attack( mario_state, interaction )

        if interaction & mario.INT_HIT_FROM_ABOVE:
            if obj.get_interaction_subtype() & mario.INT_SUBTYPE_TWIRL_BOUNCE:
                bounce_off_object( mario_state, obj, 80.0 )
                mario_state.reset_pitch()
                # TODO: Sound stuff.
                #play_sound(SOUND_MARIO_TWIRL_BOUNCE, mario_state.marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(
                    mario_state, mario.ACT_TWIRLING, 0 )

            else:
                bounce_off_object( mario_state, obj, 30.0 )

    elif take_damage_and_knock_back( mario_state, obj ):
        return True

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

def interact_damage( mario_state, interact_type, obj ):
    if take_damage_and_knock_back( mario_state, obj ):
        return True

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

def interact_breakable( mario_state, interact_type, obj ):
    interaction = determine_interaction( mario_state, obj )

    if interaction & mario.INT_ATTACK_NOT_WEAK_FROM_ABOVE:
        attack_object( obj, interaction )
        bounce_back_from_attack( mario_state, interaction )

        mario_state.set_interact_obj( obj )

        if mario.INT_HIT_FROM_ABOVE == interaction:
            bounce_off_object( mario_state, obj, 30.0 ) # Not in the 0x8F mask

        elif mario.INT_HIT_FROM_BELOW == interaction:
            hit_object_from_below( mario_state, obj )

        return True

    return False

def interact_koopa_shell( mario_state, interact_type, obj ):
    if not mario_state.get_action() & mario.ACT_FLAG_RIDING_SHELL:
        interaction = determine_interaction( mario_state, obj )

        if interaction == mario.INT_HIT_FROM_ABOVE or \
        mario_state.get_action() == mario.ACT_WALKING or \
        mario_state.action == mario.ACT_HOLD_WALKING:
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )
            mario_state.set_ridden_obj( obj )

            attack_object( obj, interaction )
            # TODO: Sound stuff.
            #mario_state.update_sound_and_camera()
            #play_shell_music();
            mario_state.drop_held_object()

            # Puts mario in ground action even when in air, making it easy to
            # escape air actions into crouch slide (shell cancel)
            return set_mario_action( mario_state, mario.ACT_RIDING_SHELL_GROUND, 0 )

        mario_state.push_out_of_object( obj, 2.0 )

    return False

def interact_pole( mario_state, interact_type, obj ):
    action_id = mario_state.get_action() & mario.ACT_ID_MASK
    if action_id >= 0x080 and action_id < 0x0A0:
        if not mario_state.get_prev_action() & mario.ACT_FLAG_ON_POLE or \
        mario_state.get_used_obj() != obj:
            low_speed = (mario_state.get_forward_vel() <= 10.0)
            mario_obj = mario_state.mario_object

            mario_state.stop_riding_and_holding()

            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )
            mario_state.set_vel( 1, 0.0 )
            mario_state.set_forward_vel( 0.0 )

            mario_obj.set_mario_pole_unk108( 0 )
            mario_obj.set_mario_pole_yaw_vel( 0 )
            mario_obj.set_mario_pole_pos(
                mario_state.get_pos( 1 ) - obj.get_pos_y() )

            if low_speed:
                return set_mario_action(
                    mario_state, mario.ACT_GRAB_POLE_SLOW, 0 )

            # @bug Using mario_state.forwardVel here is assumed to be 0.0f due to the set from earlier.
            #       This is fixed in the Shindou version.
            mario_obj.set_mario_pole_yaw_vel(
                mario_state.get_forward_vel() * 0x100 + 0x1000 )
            mario_state.reset_pitch()
            # TODO: Rumble stuff.
            # queue_rumble_data\(5, 80\);
            return set_mario_action(
                mario_state, mario.ACT_GRAB_POLE_FAST, 0 )

    return False

def interact_hoot( mario_state, interact_type, obj ):
    action_id = mario_state.get_action() & mario.ACT_ID_MASK

    # Can pause to advance the global timer without falling too far, allowing
    # you to regrab after letting go.
    if action_id >= 0x080 and \
    actionId < 0x098 and \
    mario.get_global_timer() - \
    mario_state.get_used_obj().get_hoot_mario_release_time() > 30:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_HOOT_GRABBED_BY_MARIO )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        # TODO: Sound stuff.
        #mario_state.update_sound_and_camera()
        return set_mario_action( mario_state, mario.ACT_RIDING_HOOT, 0 )

    return False

def interact_cap( mario_state, interact_type, obj ):
    cap_flag = obj.get_mario_cap_flag()
    cap_music = 0
    cap_time = 0

    if mario_state.get_action() != mario.ACT_GETTING_BLOWN and \
    cap_flag != 0:
        mario_state.set_interact_obj( obj )
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )

        mario_state.unset_flag( mario.MARIO_CAP_ON_HEAD )
        mario_state.unset_flag( mario.MARIO_CAP_IN_HAND )
        mario_state.set_flag( cap_flag )

        if MARIO_VANISH_CAP == cap_flag:
            cap_time = 600
            # TODO: Sound stuff.
            #cap_music = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP);

        elif MARIO_METAL_CAP == cap_flag:
            cap_time = 600
            # TODO: Sound stuff.
            #cap_music = SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP);

        elif MARIO_WING_CAP == cap_flag:
            cap_time = 1800
            # TODO: Sound stuff.
            #cap_music = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP);

        if cap_time > mario_state.capTimer:
            mario_state.set_cap_timer( cap_time )

        if mario_state.action & mario.ACT_FLAG_IDLE or \
        mario_state.get_action() == mario.ACT_WALKING:
            mario_state.set_flag( MARIO_CAP_IN_HAND )
            set_mario_action( mario_state, mario.ACT_PUTTING_ON_CAP, 0 )
        else:
            mario_state.set_flag( MARIO_CAP_ON_HEAD )

        # TODO: Sound stuff.
        #play_sound(SOUND_MENU_STAR_SOUND, mario_state.marioObj->header.gfx.cameraToObject);
        #play_sound(SOUND_MARIO_HERE_WE_GO, mario_state.marioObj->header.gfx.cameraToObject);
        #
        #if capMusic != 0:
        #    play_cap_music(capMusic);
        #}

        return True

    return False

def interact_grabbable( mario_state, interact_type, obj ):

    if obj.get_interaction_subtype() & mario.INT_SUBTYPE_KICKABLE:
        interaction = determine_interaction( mario_state, obj )
        if interaction & (INT_KICK | mario.INT_TRIP):
            attack_object( obj, interaction )
            bounce_back_from_attack( mario_state, interaction )
            return False

    if obj.get_interaction_subtype() & mario.INT_SUBTYPE_GRABS_MARIO:
        if check_object_grab_mario( mario_state, interact_type, obj ):
            return True

    if able_to_grab_object(m, o):
        if not obj.get_interaction_subtype() & mario.INT_SUBTYPE_NOT_GRABBABLE:
            mario_state.set_interact_obj( obj )
            mario_state.set_input( mario_state.get_input() | INPUT_INTERACT_OBJ_GRABBABLE )
            return True

    if obj.behavior_name != bhvBowser:
        mario_state.push_out_of_object( obj, -5.0 )

    return False

def interact_text( mario_state, interact_type, obj ):
    interact = False

    if obj.get_interaction_subtype() & mario.INT_SUBTYPE_SIGN:
        interact = mario_state.check_read_sign( obj )
    if obj.get_interaction_subtype() & mario.INT_SUBTYPE_NPC:
        interact = mario_state.check_npc_talk( obj )
    else:
        mario_state.push_out_of_object( obj, 2.0 )

    return interact

def interact_unknown_08( mario_state, interact_type, obj ):
    interaction = determine_interaction( mario_state, obj )

    if interaction & mario.INT_PUNCH:
        obj.set_interact_status(
            mario.INT_STATUS_INTERACTED | \
            mario.INT_STATUS_WAS_ATTACKED | \
            mario.ATTACK_PUNCH )
        bounce_back_from_attack( mario_state, interaction )
    elif take_damage_and_knock_back( mario_state, obj ):
        return True

    if not obj.get_interaction_subtype() & \
    mario.INT_SUBTYPE_DELAY_INVINCIBILITY:
        delay_invinc_timer = True

    return False

interaction_handlers = [
    InteractionHandler( mario.INTERACT_COIN,           interact_coin ),
    InteractionHandler( mario.INTERACT_WATER_RING,     interact_water_ring ),
    InteractionHandler( mario.INTERACT_STAR_OR_KEY,    interact_star_or_key ),
    InteractionHandler( mario.INTERACT_BBH_ENTRANCE,   interact_bbh_entrance ),
    InteractionHandler( mario.INTERACT_WARP,           interact_warp ),
    InteractionHandler( mario.INTERACT_WARP_DOOR,      interact_warp_door ),
    InteractionHandler( mario.INTERACT_DOOR,           interact_door ),
    InteractionHandler( mario.INTERACT_CANNON_BASE,    interact_cannon_base ),
    InteractionHandler( mario.INTERACT_IGLOO_BARRIER,  interact_igloo_barrier ),
    InteractionHandler( mario.INTERACT_TORNADO,        interact_tornado ),
    InteractionHandler( mario.INTERACT_WHIRLPOOL,      interact_whirlpool ),
    InteractionHandler( mario.INTERACT_STRONG_WIND,    interact_strong_wind ),
    InteractionHandler( mario.INTERACT_FLAME,          interact_flame ),
    InteractionHandler( mario.INTERACT_SNUFIT_BULLET,  interact_snufit_bullet ),
    InteractionHandler( mario.INTERACT_CLAM_OR_BUBBA,  interact_clam_or_bubba ),
    InteractionHandler( mario.INTERACT_BULLY,          interact_bully ),
    InteractionHandler( mario.INTERACT_SHOCK,          interact_shock ),
    InteractionHandler( mario.INTERACT_BOUNCE_TOP2,    interact_bounce_top ),
    InteractionHandler( mario.INTERACT_MR_BLIZZARD,    interact_mr_blizzard ),
    InteractionHandler( mario.INTERACT_HIT_FROM_BELOW, interact_hit_from_below ),
    InteractionHandler( mario.INTERACT_BOUNCE_TOP,     interact_bounce_top ),
    InteractionHandler( mario.INTERACT_DAMAGE,         interact_damage ),
    InteractionHandler( mario.INTERACT_POLE,           interact_pole ),
    InteractionHandler( mario.INTERACT_HOOT,           interact_hoot ),
    InteractionHandler( mario.INTERACT_BREAKABLE,      interact_breakable ),
    InteractionHandler( mario.INTERACT_KOOPA,          interact_bounce_top ),
    InteractionHandler( mario.INTERACT_KOOPA_SHELL,    interact_koopa_shell ),
    InteractionHandler( mario.INTERACT_UNKNOWN_08,     interact_unknown_08 ),
    InteractionHandler( mario.INTERACT_CAP,            interact_cap ),
    InteractionHandler( mario.INTERACT_GRABBABLE,      interact_grabbable ),
    InteractionHandler( mario.INTERACT_TEXT,           interact_text )
]

def determine_interaction( mario_state, obj ):
    interaction = 0
    action = mario_state.get_action()

    if action & mario.ACT_FLAG_ATTACKING:
        if action == mario.ACT_PUNCHING or \
        action == mario.ACT_MOVE_PUNCHING or \
        action == mario.ACT_JUMP_KICK:
            yaw_to_object = mario_state.get_angle_to_object( obj ) - \
                mario_state.get_face_angle( 1 )

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

def mario_process_interactions( mario_state ):
    delay_invinc_timer = False
    invulnerable = \
        (mario_state.get_action() & mario.ACT_FLAG_INVULNERABLE) or \
        mario_state.get_invinc_timer() != 0

    if not mario_state.get_action() & mario.ACT_FLAG_INTANGIBLE and \
    mario_state.get_collided_obj_interact_types() != 0:
        #(i = 0; i < 31; i++) {
        interact_type = 0
        for i in range( 30 ):
            interact_type = interaction_handlers[i].interact_type
            if mario_state.get_collided_obj_interact_types() & interact_type:
                obj = mario_state.get_collided_object( interact_type )

                mario_state.unset_collided_obj_interact_type( interact_type )

                if not obj.get_interact_status() & mario.INT_STATUS_INTERACTED:
                    if interaction_handlers[i].handler( mario_state, interact_type, obj ):
                        break

    if mario_state.get_invinc_timer() > 0 and not delay_invinc_timer:
        mario_state.set_invinc_timer( mario_state.get_invinc_timer() - 1 )

    # If the kick/punch flags are set and an object collision changes mario's
    # action, he will get the kick/punch wall speed anyway.
    # TODO: Port to python.
    mario_state.check_kick_or_punch_wall()
    mario_state.unset_flag( mario.MARIO_PUNCHING )
    mario_state.unset_flag( mario.MARIO_KICKING )
    mario_state.unset_flag( mario.MARIO_TRIPPING )

    mario_obj = mario_state.mario_object
    if not mario_obj.get_collided_obj_interact_types() & \
    (mario.INTERACT_WARP_DOOR | mario.INTERACT_DOOR):
        displaying_door_text = False

    if not mario_obj.get_collided_obj_interact_types() & mario.INTERACT_WARP:
        just_teleported = False

#endregion

#region actions

def determine_knockback_action( mario_state, arg ):
    bonk_action = 0

    terrain_index = 0 # 1 = air, 2 = water, 0 = default
    strength_index = 0
    interact_obj = mario_state.get_interact_obj()

    angle_to_object = mario_state.get_angle_to_object(
        interact_obj )
    facing_d_yaw = angle_to_object - mario_state.get_face_angle( 1 )
    remaining_health = \
        mario_state.get_health() - 0x40 * mario_state.get_hurt_counter()

    if mario_state.get_action() & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER):
        terrain_index = 2
    elif mario_state.get_action() & \
    (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING):
        terrain_index = 1

    if remaining_health < 0x100:
        strength_index = 2
    elif interact_obj.get_damage_or_coin_value() >= 4:
        strength_index = 2
    elif interact_obj.get_damage_or_coin_value() >= 2:
        strength_index = 1

    mario_state.set_face_angle( 1, angle_to_object )

    if terrain_index == 2:
        if mario_state.get_forward_vel() < 28.0:
            mario_state.set_forward_vel_all( 28.0 )

        if mario_state.get_pos( 1 ) >= interact_obj.get_pos_y():
            if mario_state.get_vel( 1 ) < 20.0:
                mario_state.set_vel( 1, 20.0 )
        else:
            if mario_state.get_vel( 1 ) > 0.0:
                mario_state.set_vel( 1, 0.0 )
    else:
        if mario_state.get_forward_vel() < 16.0:
            mario_state.set_forward_vel_all( 16.0 )

    if -0x4000 <= facing_d_yaw and facing_d_yaw <= 0x4000:
        mario_state.set_forward_vel( mario_state.get_forward_vel() * -1.0 )
        bonk_action = backward_knockback_actions[terrain_index][strength_index]
    else:
        mario_state.set_face_angle(
            1, mario_state.get_face_angle( 1 ) + 0x8000 )
        bonk_action = forward_knockback_actions[terrain_index][strength_index]

    return bonk_action

def drop_and_set_mario_action( mario_state, action, action_arg ):
    mario_state.stop_riding_and_holding()
    return set_mario_action( mario_state, action, action_arg )

def hurt_and_set_mario_action( mario_state, action, action_arg, hurt_counter ):
    mario_state.set_hurt_counter( hurtCounter )

    return set_mario_action(
        mario_state, action, action_arg )

def set_mario_action_moving( mario_state, action, action_arg ):

    floor_class = mario_state.get_floor_class()
    forward_vel = mario_state.get_forward_vel()
    mag = min( [mario_state.get_intended_mag(), 8.0] )

    logger.debug( "moving" )

    if mario.ACT_WALKING == action:
        if floor_class != levels.SURFACE_CLASS_VERY_SLIPPERY:
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

#endregion
