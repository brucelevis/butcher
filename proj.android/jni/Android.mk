LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/audio/include)

LOCAL_MODULE := Butcher_shared

LOCAL_MODULE_FILENAME := libButcher

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/butcher.cpp \
../../Classes/view/hud_layer.cpp \
../../Classes/view/dungeon_layer.cpp \
../../Classes/view/loading_scene.cpp \
../../Classes/view/hud_log.cpp \
../../Classes/view/game_menu.cpp \
../../Classes/actors/actor.cpp \
../../Classes/actors/object.cpp \
../../Classes/actors/item.cpp \
../../Classes/actors/character.cpp \
../../Classes/actors/player.cpp \
../../Classes/actors/monster.cpp \
../../Classes/actors/inventory.cpp \
../../Classes/actors/effects/effect.cpp \
../../Classes/actors/instances/stairs_down.cpp \
../../Classes/actors/instances/waypoint.cpp \
../../Classes/actors/instances/stairs_up.cpp \
../../Classes/actors/instances/door.cpp \
../../Classes/actors/actions/move_action.cpp \
../../Classes/actors/actions/attack_action.cpp \
../../Classes/actors/actions/die_action.cpp \
../../Classes/actors/actions/pickup_action.cpp \
../../Classes/actors/actions/shot_action.cpp \
../../Classes/actors/actions/use_action.cpp \
../../Classes/actors/ai/ai.cpp \
../../Classes/actors/ai/fsm/fsm.cpp \
../../Classes/actors/ai/fsm/states/fsm_state.cpp \
../../Classes/actors/ai/fsm/states/idle_state.cpp \
../../Classes/actors/ai/fsm/states/wandering_state.cpp \
../../Classes/actors/ai/fsm/states/move_to_target_state.cpp \
../../Classes/actors/ai/fsm/states/melee_attack_state.cpp \
../../Classes/actors/ai/fsm/states/ranged_attack_state.cpp \
../../Classes/data/actors_database.cpp \
../../Classes/data/levels_database.cpp \
../../Classes/data/effects_database.cpp \
../../Classes/dungeon/dungeon_state.cpp \
../../Classes/dungeon/level_manager.cpp \
../../Classes/dungeon/tmx_builder.cpp \
../../Classes/dungeon/room.cpp \
../../Classes/dungeon/spawn_builder.cpp \
../../Classes/dungeon/generators/celular_automata_generator.cpp \
../../Classes/dungeon/generators/minimum_corridor_generator.cpp \
../../Classes/utils/utils.cpp \
../../Classes/utils/target.cpp \
../../Classes/utils/profiler.cpp \
../../Classes/utils/directions.cpp \
../../Classes/utils/observer.cpp \
../../Classes/utils/damage.cpp \
../../Classes/utils/dices.cpp \
../../Classes/utils/path.cpp \
../../Classes/view/inventory_view.cpp \
../../Classes/view/waypoint_view.cpp \
../../Classes/actors/actions/equip_action.cpp \
../../Classes/actors/recipe.cpp \
../../Classes/data/recipes_database.cpp \
../../Classes/actors/craftbook.cpp \
../../Classes/view/craft_view.cpp \
../../Classes/view/minimap.cpp \
../../Classes/actors/actions/throw_action.cpp \
../../Classes/actors/trap.cpp \
../../Classes/lib/permissive-fov.cc

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                    $(LOCAL_PATH)/../../Classes/dungeon

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
