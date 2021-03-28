
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
'''
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
    #    # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
    #}

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
        # # TODO: Rumble stuff.
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
        save_file_collect_star_or_key( mario_state.numCoins, starIndex );

        mario_state.numStars =
            save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);

        if not no_exit:
            drop_queued_background_music();
            fadeout_level_music(126);

        play_sound(SOUND_MENU_STAR_SOUND, mario_state.marioObj->header.gfx.cameraToObject);
        mario_state.update_sound_and_camera()
        # func_802521A0

        if grand_star:
            return set_mario_action(m,mario.ACT_JUMBO_STAR_CUTSCENE, 0);

        return set_mario_action(m, starGrabAction, noExit + 2 * grandStar);

    return False

def interact_bbh_entrance( mario_state, interact_type, obj ):
    if mario_state.action != mario.ACT_BBH_ENTER_SPIN and mario_state.action != mario.ACT_BBH_ENTER_JUMP:
        mario_state.stop_riding_and_holding()

        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        if mario_state.action &mario.ACT_FLAG_AIR:
            return set_mario_action(m,mario.ACT_BBH_ENTER_SPIN, 0);
        }

        return set_mario_action(m,mario.ACT_BBH_ENTER_JUMP, 0);
    }

    return False;
}

def interact_warp( mario_state, interact_type, obj ):
    u32 action;
    just_teleported = False

    if obj.oInteractionSubtype & mario.INT_SUBTYPE_FADING_WARP:
        action = mario_state.get_action()

        if action == mario.ACT_TELEPORT_FADE_IN:
            just_teleported = True

        elif not just_teleported:
            if action == mario.ACT_IDLE or \
            action == mario.ACT_PANTING or \
            action == mario.ACT_STANDING_AGAINST_WALL or \
            action == mario.ACT_CROUCHING) {
                mario_state.set_interact_obj( obj )
                mario_state.set_used_obj( obj )

                just_teleported = True;
                return set_mario_action(m,mario.ACT_TELEPORT_FADE_OUT, 0);

    else:
        if mario_state.action != mario.ACT_EMERGE_FROM_PIPE:
            obj.set_interact_status( mario.INT_STATUS_INTERACTED )
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )

            if obj.collisionData == segmented_to_virtual(warp_pipe_seg3_collision_03009AC8):
                play_sound(SOUND_MENU_ENTER_PIPE, mario_state.marioObj->header.gfx.cameraToObject);
                # TODO: Rumble stuff.
                # queue_rumble_data\(15, 80\);
            else:
                play_sound(SOUND_MENU_ENTER_HOLE, mario_state.marioObj->header.gfx.cameraToObject);
                # TODO: Rumble stuff.
                # queue_rumble_data\(12, 80\);

            mario_state.stop_riding_object()
            return set_mario_action(m,mario.ACT_DISAPPEARED, (WARP_OP_WARP_OBJECT << 16) + 2);
        }
    }

    return False;
}

def interact_warp_door( mario_state, interact_type, obj ):
    u32 doorAction = 0;
    u32 saveFlags = save_file_get_flags();
    s16 warpDoorId = obj.oBehParams >> 24;
    u32 actionArg;

    if mario_state.action == mario.ACT_WALKING or mario_state.action == mario.ACT_DECELERATING:
        if warpDoorId == 1 and !(saveFlags & SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR):
            if !(saveFlags & SAVE_FLAG_HAVE_KEY_2):
                if !sDisplayingDoorText:
                    set_mario_action(m,mario.ACT_READING_AUTOMATIC_DIALOG,
                                     (saveFlags & SAVE_FLAG_HAVE_KEY_1) ? DIALOG_023 : DIALOG_022);
                }
                sDisplayingDoorText = True;

                return False;
            }

            doorAction = mario.ACT_UNLOCKING_KEY_DOOR;
        }

        if warpDoorId == 2 and !(saveFlags & SAVE_FLAG_UNLOCKED_BASEMENT_DOOR):
            if !(saveFlags & SAVE_FLAG_HAVE_KEY_1):
                if !sDisplayingDoorText:
                    // Moat door skip was intended confirmed
                    set_mario_action(m,mario.ACT_READING_AUTOMATIC_DIALOG,
                                     (saveFlags & SAVE_FLAG_HAVE_KEY_2) ? DIALOG_023 : DIALOG_022);
                }
                sDisplayingDoorText = True;

                return False;
            }

            doorAction = mario.ACT_UNLOCKING_KEY_DOOR;
        }

        if mario_state.action == mario.ACT_WALKING or mario_state.action == mario.ACT_DECELERATING:
            actionArg = should_push_or_pull_door(m, o) + 0x00000004;

            if doorAction == 0:
                if actionArg & 0x00000001:
                    doorAction = mario.ACT_PULLING_DOOR;
                else:
                    doorAction = mario.ACT_PUSHING_DOOR;
                }
            }

            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )
            return set_mario_action(m, doorAction, actionArg);
        }
    }

    return False;
}

def get_door_save_file_flag(struct Object *door) {
    u32 saveFileFlag = 0;
    s16 requiredNumStars = door->oBehParams >> 24;

    s16 isCcmDoor = door->oPosX < 0.0f;
    s16 isPssDoor = door->oPosY > 500.0f;

    switch (requiredNumStars) {
        case 1:
            if isPssDoor:
                saveFileFlag = SAVE_FLAG_UNLOCKED_PSS_DOOR;
            else:
                saveFileFlag = SAVE_FLAG_UNLOCKED_WF_DOOR;
            }
            break;

        case 3:
            if isCcmDoor:
                saveFileFlag = SAVE_FLAG_UNLOCKED_CCM_DOOR;
            else:
                saveFileFlag = SAVE_FLAG_UNLOCKED_JRB_DOOR;
            }
            break;

        case 8:
            saveFileFlag = SAVE_FLAG_UNLOCKED_BITDW_DOOR;
            break;

        case 30:
            saveFileFlag = SAVE_FLAG_UNLOCKED_BITFS_DOOR;
            break;

        case 50:
            saveFileFlag = SAVE_FLAG_UNLOCKED_50_STAR_DOOR;
            break;
    }

    return saveFileFlag;
}

def interact_door( mario_state, interact_type, obj ):
    s16 requiredNumStars = obj.oBehParams >> 24;
    s16 numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);

    if mario_state.action == mario.ACT_WALKING or mario_state.action == mario.ACT_DECELERATING:
        if numStars >= requiredNumStars:
            u32 actionArg = should_push_or_pull_door(m, o);
            u32 enterDoorAction;
            u32 doorSaveFileFlag;

            if actionArg & 0x00000001:
                enterDoorAction = mario.ACT_PULLING_DOOR;
            else:
                enterDoorAction = mario.ACT_PUSHING_DOOR;
            }

            doorSaveFileFlag = get_door_save_file_flag(o);
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )

            if obj.oInteractionSubtype & mario.INT_SUBTYPE_STAR_DOOR:
                enterDoorAction = mario.ACT_ENTERING_STAR_DOOR;
            }

            if doorSaveFileFlag != 0 and !(save_file_get_flags() & doorSaveFileFlag):
                enterDoorAction = mario.ACT_UNLOCKING_STAR_DOOR;
            }

            return set_mario_action(m, enterDoorAction, actionArg);
        elif !sDisplayingDoorText:
            u32 text = DIALOG_022 << 16;

            switch (requiredNumStars) {
                case 1:
                    text = DIALOG_024 << 16;
                    break;
                case 3:
                    text = DIALOG_025 << 16;
                    break;
                case 8:
                    text = DIALOG_026 << 16;
                    break;
                case 30:
                    text = DIALOG_027 << 16;
                    break;
                case 50:
                    text = DIALOG_028 << 16;
                    break;
                case 70:
                    text = DIALOG_029 << 16;
                    break;
            }

            text += requiredNumStars - numStars;

            sDisplayingDoorText = True;
            return set_mario_action(m,mario.ACT_READING_AUTOMATIC_DIALOG, text);
        }
    elif mario_state.action == mario.ACT_IDLE and sDisplayingDoorText == True and requiredNumStars == 70:
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )
        return set_mario_action(m,mario.ACT_ENTERING_STAR_DOOR, should_push_or_pull_door(m, o));
    }

    return False;
}

def interact_cannon_base( mario_state, interact_type, obj ):
    if mario_state.action != mario.ACT_IN_CANNON:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )
        return set_mario_action(m,mario.ACT_IN_CANNON, 0);
    }

    return False;
}

def interact_igloo_barrier( mario_state, interact_type, obj ):
    //! Sets used object without changing action (LOTS of interesting glitches,
    // but unfortunately the igloo barrier is the only object with this interaction
    // type)
    mario_state.set_interact_obj( obj )
    mario_state.set_used_obj( obj )
    push_mario_out_of_object(m, o, 5.0f);
    return False;
}

def interact_tornado( mario_state, interact_type, obj ):
    struct Object *marioObj = mario_state.marioObj;

    if mario_state.action != mario.ACT_TORNADO_TWIRLING and mario_state.action != mario.ACT_SQUISHED:
        mario_state.stop_riding_and_holding()
        mario_state.set_forward_vel_all( 0.0 )
        mario_state.update_sound_and_camera()

        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        marioObj->oMarioTornadoYawVel = 0x400;
        marioObj->oMarioTornadoPosY = mario_state.pos[1] - obj.oPosY;

        play_sound(SOUND_MARIO_WAAAOOOW, mario_state.marioObj->header.gfx.cameraToObject);
        # TODO: Rumble stuff.
        # queue_rumble_data\(30, 60\);
        
        return set_mario_action(m,mario.ACT_TORNADO_TWIRLING, mario_state.action == mario.ACT_TWIRLING);
    }

    return False;
}

def interact_whirlpool( mario_state, interact_type, obj ):
    struct Object *marioObj = mario_state.marioObj;

    if mario_state.action != mario.ACT_CAUGHT_IN_WHIRLPOOL:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        mario_state.set_forward_vel( 0.0 )

        marioObj->oMarioWhirlpoolPosY = mario_state.pos[1] - obj.oPosY;

        play_sound(SOUND_MARIO_WAAAOOOW, mario_state.marioObj->header.gfx.cameraToObject);
        # TODO: Rumble stuff.
        # queue_rumble_data\(30, 60\);
        
        return set_mario_action(m,mario.ACT_CAUGHT_IN_WHIRLPOOL, 0);
    }

    return False;
}

def interact_strong_wind( mario_state, interact_type, obj ):
    UNUSED struct Object *marioObj = mario_state.marioObj;

    if mario_state.action != mario.ACT_GETTING_BLOWN:
        mario_state.stop_riding_and_holding()
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        mario_state.faceAngle[1] = obj.oMoveAngleYaw + 0x8000;
        mario_state.unkC4 = 0.4f;
        mario_state.forwardVel = -24.0f;
        mario_state.vel[1] = 12.0f;

        play_sound(SOUND_MARIO_WAAAOOOW, mario_state.marioObj->header.gfx.cameraToObject);
        mario_state.update_sound_and_camera()
        return set_mario_action(m,mario.ACT_GETTING_BLOWN, 0);
    }

    return False;
}

def interact_flame( mario_state, interact_type, obj ):
    u32 burningAction = mario.ACT_BURNING_JUMP;

    if (!sInvulnerable and !(mario_state.get_flags() & MARIO_METAL_CAP) and !(mario_state.get_flags() & MARIO_VANISH_CAP)
        and !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )

        if ((mario_state.action & (ACT_FLAG_SWIMMING |mario.ACT_FLAG_METAL_WATER))
            or mario_state.waterLevel - mario_state.pos[1] > 50.0f) {
            play_sound(SOUND_GENERAL_FLAME_OUT, mario_state.marioObj->header.gfx.cameraToObject);
        else:
            mario_state.marioObj->oMarioBurnTimer = 0;
            mario_state.update_sound_and_camera()
            play_sound(SOUND_MARIO_ON_FIRE, mario_state.marioObj->header.gfx.cameraToObject);

            if (mario_state.action &mario.ACT_FLAG_AIR) and mario_state.vel[1] <= 0.0f:
                burningAction = mario.ACT_BURNING_FALL;
            }

            return drop_and_set_mario_action(m, burningAction, 1);
        }
    }

    return False;
}

def interact_snufit_bullet( mario_state, interact_type, obj ):
    if !sInvulnerable and !(mario_state.get_flags() & MARIO_VANISH_CAP):
        if mario_state.get_flags() & MARIO_METAL_CAP:
            obj.oInteractStatus = mario.INT_STATUS_INTERACTED | mario.INT_STATUS_WAS_ATTACKED;
            play_sound(SOUND_ACTION_UNKNOWN458, mario_state.marioObj->header.gfx.cameraToObject);
        else:
            obj.oInteractStatus = mario.INT_STATUS_INTERACTED | mario.INT_STATUS_ATTACKED_MARIO;
            mario_state.set_interact_obj( obj )
            take_damage_from_interact_object(m);

            play_sound(SOUND_MARIO_ATTACKED, mario_state.marioObj->header.gfx.cameraToObject);
            mario_state.update_sound_and_camera()

            return drop_and_set_mario_action(m, determine_knockback_action(m, obj.oDamageOrCoinValue),
                                             obj.oDamageOrCoinValue);
        }
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }

    return False;
}

def interact_clam_or_bubba( mario_state, interact_type, obj ):
    if obj.oInteractionSubtype & mario.INT_SUBTYPE_EATS_MARIO:
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.set_interact_obj( obj )
        return set_mario_action(m,mario.ACT_EATEN_BY_BUBBA, 0);
    elif take_damage_and_knock_back(m, o):
        return True;
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }
    return True;
}

def interact_bully( mario_state, interact_type, obj ):
    UNUSED u32 unused;

    u32 interaction;
    if mario_state.get_flags() & MARIO_METAL_CAP:
        interaction = mario.INT_FAST_ATTACK_OR_SHELL;
    else:
        interaction = determine_interaction(m, o);
    }

    mario_state.set_interact_obj( obj )

    if interaction & mario.INT_ATTACK_NOT_FROM_BELOW:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        push_mario_out_of_object(m, o, 5.0f);

        mario_state.forwardVel = -16.0f;
        obj.oMoveAngleYaw = mario_state.faceAngle[1];
        obj.oForwardVel = 3392.0f / obj.hitboxRadius;

        attack_object(o, interaction);
        bounce_back_from_attack(m, interaction);
        return True;
    }

    else if (!sInvulnerable and !(mario_state.get_flags() & MARIO_VANISH_CAP)
             and !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )
        mario_state.invincTimer = 2;

        mario_state.update_sound_and_camera()
        play_sound(SOUND_MARIO_EEUH, mario_state.marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_OBJ_BULLY_METAL, mario_state.marioObj->header.gfx.cameraToObject);

        push_mario_out_of_object(m, o, 5.0f);
        drop_and_set_mario_action(m, bully_knock_back_mario(m), 0);
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        
        return True;
    }

    return False;
}

def interact_shock( mario_state, interact_type, obj ):
    if (!sInvulnerable and !(mario_state.get_flags() & MARIO_VANISH_CAP)
        and !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        u32 actionArg = (mario_state.action & (ACT_FLAG_AIR |mario.ACT_FLAG_ON_POLE |mario.ACT_FLAG_HANGING)) == 0;

        obj.oInteractStatus = mario.INT_STATUS_INTERACTED | mario.INT_STATUS_ATTACKED_MARIO;
        mario_state.set_interact_obj( obj )

        take_damage_from_interact_object(m);
        play_sound(SOUND_MARIO_ATTACKED, mario_state.marioObj->header.gfx.cameraToObject);
        # TODO: Rumble stuff.
        # queue_rumble_data\(70, 60\);

        if mario_state.action & (ACT_FLAG_SWIMMING |mario.ACT_FLAG_METAL_WATER):
            return drop_and_set_mario_action(m,mario.ACT_WATER_SHOCKED, 0);
        else:
            mario_state.update_sound_and_camera()
            return drop_and_set_mario_action(m,mario.ACT_SHOCKED, actionArg);
        }
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }
    return False;
}

def interact_mr_blizzard( mario_state, interact_type, obj ):
    if take_damage_and_knock_back(m, o):
        return True;
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }

    return False;
}

def interact_hit_from_below( mario_state, interact_type, obj ):
    UNUSED u32 unused;

    interaction = 0
    if mario_state.get_flags() & MARIO_METAL_CAP:
        interaction = mario.INT_FAST_ATTACK_OR_SHELL
    else:
        interaction = determine_interaction( mario_state, obj )

    if interaction & mario.INT_ANY_ATTACK:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        attack_object(o, interaction);
        bounce_back_from_attack(m, interaction);

        if interaction & mario.INT_HIT_FROM_BELOW:
            hit_object_from_below(m, o);
        }

        if interaction & mario.INT_HIT_FROM_ABOVE:
            if obj.oInteractionSubtype & mario.INT_SUBTYPE_TWIRL_BOUNCE:
                bounce_off_object(m, o, 80.0f);
                mario_state.reset_pitch()
                play_sound(SOUND_MARIO_TWIRL_BOUNCE, mario_state.marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(m,mario.ACT_TWIRLING, 0);
            else:
                bounce_off_object(m, o, 30.0f);
            }
        }
    elif take_damage_and_knock_back(m, o):
        return True;
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }
    return False;
}

def interact_bounce_top( mario_state, interact_type, obj ):
    u32 interaction;
    if mario_state.get_flags() & MARIO_METAL_CAP:
        interaction = mario.INT_FAST_ATTACK_OR_SHELL;
    else:
        interaction = determine_interaction(m, o);
    }

    if interaction & mario.INT_ATTACK_NOT_FROM_BELOW:
        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        attack_object(o, interaction);
        bounce_back_from_attack(m, interaction);

        if interaction & mario.INT_HIT_FROM_ABOVE:
            if obj.oInteractionSubtype & mario.INT_SUBTYPE_TWIRL_BOUNCE:
                bounce_off_object(m, o, 80.0f);
                mario_state.reset_pitch()
                play_sound(SOUND_MARIO_TWIRL_BOUNCE, mario_state.marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(m,mario.ACT_TWIRLING, 0);
            else:
                bounce_off_object(m, o, 30.0f);
            }
        }
    elif take_damage_and_knock_back(m, o):
        return True;
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }
    return False;
}

def interact_damage( mario_state, interact_type, obj ):
    if take_damage_and_knock_back(m, o):
        return True;
    }

    if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_DELAY_INVINCIBILITY):
        sDelayInvincTimer = True;
    }
    return False;
}

def interact_breakable( mario_state, interact_type, obj ):
    u32 interaction = determine_interaction(m, o);

    if interaction & mario.INT_ATTACK_NOT_WEAK_FROM_ABOVE:
        attack_object(o, interaction);
        bounce_back_from_attack(m, interaction);

        mario_state.set_interact_obj( obj )

        switch (interaction) {
            case mario.INT_HIT_FROM_ABOVE:
                bounce_off_object(m, o, 30.0f); //! Not in the 0x8F mask
                break;

            case mario.INT_HIT_FROM_BELOW:
                hit_object_from_below(m, o);
                break;
        }

        return True;
    }

    return False;
}

def interact_koopa_shell( mario_state, interact_type, obj ):
    if !(mario_state.action &mario.ACT_FLAG_RIDING_SHELL):
        u32 interaction = determine_interaction(m, o);

        if (interaction == mario.INT_HIT_FROM_ABOVE or mario_state.action == mario.ACT_WALKING
            or mario_state.action == mario.ACT_HOLD_WALKING) {
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )
            mario_state.riddenObj = o;

            attack_object(o, interaction);
            mario_state.update_sound_and_camera()
            play_shell_music();
            mario_drop_held_object(m);

            //! Puts mario in ground action even when in air, making it easy to
            // escape air actions into crouch slide (shell cancel)
            return set_mario_action(m,mario.ACT_RIDING_SHELL_GROUND, 0);
        }

        push_mario_out_of_object(m, o, 2.0f);
    }

    return False;
}

def check_object_grab_mario( mario_state, interact_type, obj ):
    if ((!(mario_state.action & (ACT_FLAG_AIR |mario.ACT_FLAG_INVULNERABLE |mario.ACT_FLAG_ATTACKING)) or !sInvulnerable)
        and (obj.oInteractionSubtype & mario.INT_SUBTYPE_GRABS_MARIO)) {
        if object_facing_mario(m, o, 0x2AAA):
            mario_state.stop_riding_and_holding()
            obj.oInteractStatus = mario.INT_STATUS_INTERACTED | mario.INT_STATUS_GRABBED_MARIO;

            mario_state.faceAngle[1] = obj.oMoveAngleYaw;
            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )

            mario_state.update_sound_and_camera()
            play_sound(SOUND_MARIO_OOOF, mario_state.marioObj->header.gfx.cameraToObject);
            # TODO: Rumble stuff.
            # queue_rumble_data\(5, 80\);
            return set_mario_action(m,mario.ACT_GRABBED, 0);
        }
    }

    push_mario_out_of_object(m, o, -5.0f);
    return False;
}

def interact_pole( mario_state, interact_type, obj ):
    s32 actionId = mario_state.action &mario.ACT_ID_MASK;
    if actionId >= 0x080 and actionId < 0x0A0:
        if !(mario_state.prevAction &mario.ACT_FLAG_ON_POLE) or mario_state.usedObj != o:
            low_speed = (mario_state.get_forward_vel() <= 10.0)
            struct Object *marioObj = mario_state.marioObj;

            mario_state.stop_riding_and_holding()

            mario_state.set_interact_obj( obj )
            mario_state.set_used_obj( obj )
            mario_state.set_vel( 1, 0.0 )
            mario_state.set_forward_vel( 0.0 )

            marioObj->oMarioPoleUnk108 = 0;
            marioObj->oMarioPoleYawVel = 0;
            marioObj->oMarioPolePos = mario_state.pos[1] - obj.oPosY;

            if low_speed:
                return set_mario_action(m,mario.ACT_GRAB_POLE_SLOW, 0);

            # @bug Using mario_state.forwardVel here is assumed to be 0.0f due to the set from earlier.
            #       This is fixed in the Shindou version.
            marioObj->oMarioPoleYawVel = (s32)(mario_state.forwardVel * 0x100 + 0x1000);
            mario_state.reset_pitch()
            # TODO: Rumble stuff.
            # queue_rumble_data\(5, 80\);
            return set_mario_action(m,mario.ACT_GRAB_POLE_FAST, 0);
        }
    }

    return False;
}

def interact_hoot( mario_state, interact_type, obj ):
    s32 actionId = mario_state.action &mario.ACT_ID_MASK;

    # Can pause to advance the global timer without falling too far, allowing
    # you to regrab after letting go.
    if (actionId >= 0x080 and actionId < 0x098
        and (gGlobalTimer - mario_state.usedObj->oHootMarioReleaseTime > 30)) {
        mario_state.stop_riding_and_holding()
        obj.oInteractStatus = mario.INT_STATUS_HOOT_GRABBED_BY_MARIO;
        mario_state.set_interact_obj( obj )
        mario_state.set_used_obj( obj )

        # TODO: Rumble stuff.
        # queue_rumble_data\(5, 80\);
        mario_state.update_sound_and_camera()
        return set_mario_action(m,mario.ACT_RIDING_HOOT, 0);
    }

    return False;
}

def interact_cap( mario_state, interact_type, obj ):
    u32 capFlag = get_mario_cap_flag(o);
    u16 capMusic = 0;
    u16 capTime = 0;

    if mario_state.get_action() != mario.ACT_GETTING_BLOWN and cap_flag != 0:
        mario_state.set_interact_obj( obj )
        obj.set_interact_status( mario.INT_STATUS_INTERACTED )

        mario_state.unset_flag( mario.MARIO_CAP_ON_HEAD )
        mario_state.unset_flag( mario.MARIO_CAP_IN_HAND )
        mario_state.set_flag( cap_flag )

        switch (capFlag) {
            case MARIO_VANISH_CAP:
                capTime = 600;
                capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP);
                break;

            case MARIO_METAL_CAP:
                capTime = 600;
                capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP);
                break;

            case MARIO_WING_CAP:
                capTime = 1800;
                capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP);
                break;
        }

        if capTime > mario_state.capTimer:
            mario_state.capTimer = capTime;
        }

        if (mario_state.action &mario.ACT_FLAG_IDLE) or mario_state.action == mario.ACT_WALKING:
            mario_state.flags |= MARIO_CAP_IN_HAND;
            set_mario_action(m,mario.ACT_PUTTING_ON_CAP, 0);
        else:
            mario_state.flags |= MARIO_CAP_ON_HEAD;
        }

        play_sound(SOUND_MENU_STAR_SOUND, mario_state.marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_MARIO_HERE_WE_GO, mario_state.marioObj->header.gfx.cameraToObject);

        if capMusic != 0:
            play_cap_music(capMusic);
        }

        return True;
    }

    return False;
}

def interact_grabbable( mario_state, interact_type, obj ):

    if obj.oInteractionSubtype & mario.INT_SUBTYPE_KICKABLE:
        u32 interaction = determine_interaction(m, o);
        if interaction & (INT_KICK | mario.INT_TRIP):
            attack_object(o, interaction);
            bounce_back_from_attack(m, interaction);
            return False;
        }
    }

    if (obj.oInteractionSubtype & mario.INT_SUBTYPE_GRABS_MARIO):
        if check_object_grab_mario(m, interactType, o):
            return True

    if able_to_grab_object(m, o):
        if !(obj.oInteractionSubtype & mario.INT_SUBTYPE_NOT_GRABBABLE):
            mario_state.set_interact_obj( obj )
            mario_state.input |= INPUT_INTERACT_OBJ_GRABBABLE;
            return True

    if obj.behavior_name != bhvBowser:
        mario_state.push_out_of_object( obj, -5.0 )
    }
    return False
}

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
'''

#endregion

#region actions

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
