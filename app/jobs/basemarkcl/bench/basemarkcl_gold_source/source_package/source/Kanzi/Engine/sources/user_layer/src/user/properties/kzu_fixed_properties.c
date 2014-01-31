/**
 * \file
 * Fixed property types.
 * Do not change this file, as the file is generated automatically.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#include "kzu_fixed_properties.h"

#include "kzu_property.h"
#include "kzu_float_property.h"
#include "kzu_bool_property.h"
#include "kzu_int_property.h"
#include "kzu_vector2_property.h"
#include "kzu_vector3_property.h"
#include "kzu_vector4_property.h"
#include "kzu_matrix2x2_property.h"
#include "kzu_matrix3x3_property.h"
#include "kzu_matrix4x4_property.h"
#include "kzu_string_property.h"
#include "kzu_void_property.h"
#include "kzu_color_property.h"
#include "kzu_texture_property.h"
#include "kzu_light_property.h"

#include <core/memory/kzc_memory_manager.h>
#include <core/util/color/kzc_color.h>
#include <core/util/math/kzc_vector2.h>
#include <core/util/math/kzc_vector3.h>
#include <core/util/math/kzc_vector4.h>
#include <core/util/math/kzc_matrix2x2.h>
#include <core/util/math/kzc_matrix3x3.h>
#include <core/util/math/kzc_matrix4x4.h>
#include <core/util/collection/kzc_hash_map.h>


/*lint --e{956} Suppress warning of global non-const variables. */

static struct KzcHashMap* g_fixedProperties = KZ_NULL;


const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ASPECT_RATIO;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_DISABLE_ASPECT_RATIO;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ZNEAR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ZFAR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_PLANE_HEIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_FOV_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_FOV;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ORIENTATION_ANGLE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SCENE_STARTUP_SCRIPT_FUNCTION_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRANSFORMATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FRUSTUM_CULLING_ON;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_RENDER_PASS_IS_2D;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_WRITE_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_TEST_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_WRITE_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_FUNCTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_WRITE_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_BUFFER_TEST_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_INTENSITY;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_SPREAD;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_BOX_BLUR_SPREAD;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_BRIGHTNESS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_CONTRAST;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_GAMMA_CORRECT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_HUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_LIGHTNESS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_SATURATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_SELECTED_COMPOSER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_AREA_LENGTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFBLUR_SPREAD;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_FADE_OFF;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_DISTANCE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_MOTION_BLUR_STRENGTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_RADIAL_BLUR_AMOUNT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_RENDERING_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_EYE_DISTANCE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_FOCUS_DISTANCE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_FADE_SPEED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_POWER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_XSCALE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_YSCALE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_FAR_PLANE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_NEAR_PLANE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_SHADOW_CASTER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_IS_SHADOW_PASS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_VIEW_ANGLE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LIGHT_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_DIRECTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_SHADOW_CASTER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT_ATTENUATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT_POSITION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_SHADOW_CASTER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_ATTENUATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_CUTOFF_ANGLE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_DIRECTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_EXPONENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_POSITION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GLOBAL_AMBIENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_DENSITY;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_END;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_START;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_AMBIENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_BLEND_INTENSITY;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_BLEND_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIFFUSE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_EMISSIVE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TEXTURE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_INTENSITY;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPECULAR_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPECULAR_EXPONENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_RENDER_PASS_CAMERA_CAMERA;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_RENDER_PASS_CAMERA_RENDER_PASS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_COMPOSER_COMPOSER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_COMPOSER_SCENE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_RENDER_PASS_ACTION_RENDER_PASS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_RENDER_PASS_ACTION_ENABLED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_2DPICKING_CAMERA_SCENE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_2DPICKING_CAMERA_CAMERA;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_SCENE_CAMERA_SCENE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_SCENE_CAMERA_CAMERA;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_PLAYBACK_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_DURATION_SCALE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_REPEAT_COUNT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_HOST_ITEM;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_ATTRIBUTE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_ITEM_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_REFERENCE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_TARGET_SCENE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_TARGET_CAMERA;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_DURATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_ITEM;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_PROPERTY_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_PROPERTY_ATTRIBUTE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_BOOLEAN_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TEXTURE_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_TEXTURE_TARGET_ITEM;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_ITEM_TO_RESET;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VISIBLE_AMOUNT_IN_PARENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_ANIMATION_ITEM_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CLIP_CHILDREN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_COMPONENT_PRESSED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CONTENT_CONTAINER;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CULL_MODE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_CUSTOM_NODE_TYPE_ID;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STACK_LAYOUT_DIRECTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_DIRECTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_EXPAND_DIRECTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FACE_TO_CAMERA;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SCRIPT_FUNCTION_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_INVERT_DIRECTION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_ITEMS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LOOK_AT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_LOOPING;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PICKABLE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PICKABLE_2D;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_PLAY_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STACK_LAYOUT_REVERSED;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SCENE_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_INDEX;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_INT_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VISIBLE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_FLOAT_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_DEFINITIONS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_DEFINITIONS;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_START_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_END_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_OFFSET;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_WIDTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HEIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_ALIGNMENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_ALIGNMENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH_ALIGNMENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_MARGIN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_MARGIN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH_MARGIN;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_WIDTH_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HEIGHT_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_BUTTON_LONG_RELEASE_INTERVAL;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_BUTTON_REPEAT_INTERVAL;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_COLOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_FONT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_FONT_SIZE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_LINE_SPACING;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_CHARACTERS_PER_LINE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_HEIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_LINES;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_WIDTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_TERMINATOR;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_TEXT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_HORIZONTAL_ALIGNMENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_VERTICAL_ALIGNMENT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_MINIMUM_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_MAXIMUM_VALUE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_NEW_SELECTED_ITEM;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_OLD_SELECTED_ITEM;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADER_TRANSITION_MATERIAL;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_WIDTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_HEIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_X;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_Y;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_WIDTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_HEIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_STARTUP_SCENE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_METRICS_TYPE;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_X;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_Y;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_WIDTH;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_HEIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_ASPECT_RATIO;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT_NAME;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_DATA;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_TARGET_COMPONENT_LOCATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT;
const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT;


static kzsError kzuFixedPropertiesCreateLightTypes_internal(const struct KzcMemoryManager* memoryManager)
{
    kzsError result;

    /* Directional light. */
    {
        struct KzuLightPropertyType* propertyTypeLight;
        struct KzuPropertyType* propertyType;
        result = kzuLightPropertyTypeCreate(memoryManager, "DirectionalLight", &propertyTypeLight);
        kzsErrorForward(result);
        propertyType = kzuLightPropertyTypeToPropertyType(propertyTypeLight);
        result = kzcHashMapPut(g_fixedProperties, "DirectionalLight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT = propertyType;

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_COLOR);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_DIRECTION);
        kzsErrorForward(result);
    }

    /* Point light. */
    {
        struct KzuLightPropertyType* propertyTypeLight;
        struct KzuPropertyType* propertyType;
        result = kzuLightPropertyTypeCreate(memoryManager, "PointLight", &propertyTypeLight);
        kzsErrorForward(result);
        propertyType = kzuLightPropertyTypeToPropertyType(propertyTypeLight);
        result = kzcHashMapPut(g_fixedProperties, "PointLight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POINT_LIGHT = propertyType;

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_POINT_LIGHT_COLOR);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_POINT_LIGHT_POSITION);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_POINT_LIGHT_ATTENUATION);
        kzsErrorForward(result);
    }

    /* Spot light. */
    {
        struct KzuLightPropertyType* propertyTypeLight;
        struct KzuPropertyType* propertyType;
        result = kzuLightPropertyTypeCreate(memoryManager, "SpotLight", &propertyTypeLight);
        kzsErrorForward(result);
        propertyType = kzuLightPropertyTypeToPropertyType(propertyTypeLight);
        result = kzcHashMapPut(g_fixedProperties, "SpotLight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT = propertyType;

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_SPOT_LIGHT_COLOR);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_SPOT_LIGHT_POSITION);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_SPOT_LIGHT_DIRECTION);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_SPOT_LIGHT_ATTENUATION);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_SPOT_LIGHT_CUTOFF_ANGLE);
        kzsErrorForward(result);

        result = kzuLightPropertyTypeAddProperty(propertyTypeLight, KZU_PROPERTY_TYPE_SPOT_LIGHT_EXPONENT);
        kzsErrorForward(result);
    }

    kzsSuccess();
}

kzsError kzuFixedPropertiesCreate(const struct KzcMemoryManager* memoryManager)
{
    kzsError result;
    
    result = kzcHashMapCreate(memoryManager, KZC_HASH_MAP_CONFIGURATION_STRING, &g_fixedProperties);
    kzsErrorForward(result);
    
    /* CameraAspectRatio */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "CameraAspectRatio", 1.666667f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraAspectRatio", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_ASPECT_RATIO = propertyType;
    }
    
    /* CameraDisableAspectRatio */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "CameraDisableAspectRatio", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraDisableAspectRatio", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_DISABLE_ASPECT_RATIO = propertyType;
    }
    
    /* CameraZNear */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "CameraZNear", 0.1f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraZNear", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_ZNEAR = propertyType;
    }
    
    /* CameraZFar */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "CameraZFar", 100.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraZFar", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_ZFAR = propertyType;
    }
    
    /* CameraProjectionType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "CameraProjectionType", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraProjectionType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE = propertyType;
    }
    
    /* CameraOrthogonalCoordinateSystemType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "CameraOrthogonalCoordinateSystemType", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraOrthogonalCoordinateSystemType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE = propertyType;
    }
    
    /* CameraOrthogonalPlaneHeight */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "CameraOrthogonalPlaneHeight", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraOrthogonalPlaneHeight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_PLANE_HEIGHT = propertyType;
    }
    
    /* CameraFovType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "CameraFovType", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraFovType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_FOV_TYPE = propertyType;
    }
    
    /* CameraFov */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "CameraFov", 45.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraFov", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_FOV = propertyType;
    }
    
    /* CameraOrientationAngle */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "CameraOrientationAngle", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CameraOrientationAngle", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CAMERA_ORIENTATION_ANGLE = propertyType;
    }
    
    /* SceneStartupScriptFunctionName */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "SceneStartupScriptFunctionName", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SceneStartupScriptFunctionName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SCENE_STARTUP_SCRIPT_FUNCTION_NAME = propertyType;
    }
    
    /* Transformation */
    {
        struct KzuMatrix4x4PropertyType* propertyTypeMatrix4x4;
        struct KzuPropertyType* propertyType;
        result = kzuMatrix4x4PropertyTypeCreate(memoryManager, "Transformation", kzcMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f), &propertyTypeMatrix4x4);
        kzsErrorForward(result);
        propertyType = kzuMatrix4x4PropertyTypeToPropertyType(propertyTypeMatrix4x4);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "Transformation", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TRANSFORMATION = propertyType;
    }
    
    /* FrustumCullingOn */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "FrustumCullingOn", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "FrustumCullingOn", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FRUSTUM_CULLING_ON = propertyType;
    }
    
    /* RenderPassIs2d */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "RenderPassIs2d", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "RenderPassIs2d", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_RENDER_PASS_IS_2D = propertyType;
    }
    
    /* ColorBufferClearColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "ColorBufferClearColor", kzcColorRGBA(0.4f, 0.4f, 0.4f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ColorBufferClearColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_COLOR = propertyType;
    }
    
    /* ColorBufferClearEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "ColorBufferClearEnabled", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ColorBufferClearEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_ENABLED = propertyType;
    }
    
    /* ColorWriteMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "ColorWriteMode", 2, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ColorWriteMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_COLOR_WRITE_MODE = propertyType;
    }
    
    /* DepthBufferClearEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "DepthBufferClearEnabled", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DepthBufferClearEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_ENABLED = propertyType;
    }
    
    /* DepthBufferClearValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "DepthBufferClearValue", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DepthBufferClearValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_VALUE = propertyType;
    }
    
    /* DepthBufferTestEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "DepthBufferTestEnabled", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DepthBufferTestEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DEPTH_BUFFER_TEST_ENABLED = propertyType;
    }
    
    /* DepthBufferWriteEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "DepthBufferWriteEnabled", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DepthBufferWriteEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DEPTH_BUFFER_WRITE_ENABLED = propertyType;
    }
    
    /* StencilBufferClearEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "StencilBufferClearEnabled", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StencilBufferClearEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_ENABLED = propertyType;
    }
    
    /* StencilBufferClearValue */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "StencilBufferClearValue", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StencilBufferClearValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_VALUE = propertyType;
    }
    
    /* StencilFunction */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "StencilFunction", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StencilFunction", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STENCIL_FUNCTION = propertyType;
    }
    
    /* StencilWriteMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "StencilWriteMode", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StencilWriteMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STENCIL_WRITE_MODE = propertyType;
    }
    
    /* StencilBufferTestEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "StencilBufferTestEnabled", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StencilBufferTestEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STENCIL_BUFFER_TEST_ENABLED = propertyType;
    }
    
    /* PostProcessingBloomIntensity */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "PostProcessingBloomIntensity", kzcColorRGBA(0.5f, 0.5f, 0.5f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingBloomIntensity", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_INTENSITY = propertyType;
    }
    
    /* PostProcessingBloomSpread */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingBloomSpread", 0.05f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingBloomSpread", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_SPREAD = propertyType;
    }
    
    /* PostProcessingBoxBlurSpread */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingBoxBlurSpread", 50.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingBoxBlurSpread", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_BOX_BLUR_SPREAD = propertyType;
    }
    
    /* PostProcessingColorAdjustBrightness */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingColorAdjustBrightness", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingColorAdjustBrightness", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_BRIGHTNESS = propertyType;
    }
    
    /* PostProcessingColorAdjustContrast */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingColorAdjustContrast", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingColorAdjustContrast", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_CONTRAST = propertyType;
    }
    
    /* PostProcessingColorAdjustGammaCorrect */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingColorAdjustGammaCorrect", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingColorAdjustGammaCorrect", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_GAMMA_CORRECT = propertyType;
    }
    
    /* PostProcessingColorAdjustHue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingColorAdjustHue", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingColorAdjustHue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_HUE = propertyType;
    }
    
    /* PostProcessingColorAdjustLightness */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingColorAdjustLightness", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingColorAdjustLightness", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_LIGHTNESS = propertyType;
    }
    
    /* PostProcessingColorAdjustSaturation */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingColorAdjustSaturation", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingColorAdjustSaturation", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_SATURATION = propertyType;
    }
    
    /* PostProcessingSelectedComposer */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "PostProcessingSelectedComposer", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingSelectedComposer", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_SELECTED_COMPOSER = propertyType;
    }
    
    /* PostProcessingDOFFocusAreaLength */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingDOFFocusAreaLength", 10.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingDOFFocusAreaLength", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_AREA_LENGTH = propertyType;
    }
    
    /* PostProcessingDOFBlurSpread */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingDOFBlurSpread", 0.05f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingDOFBlurSpread", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_DOFBLUR_SPREAD = propertyType;
    }
    
    /* PostProcessingDOFFocusFadeOff */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingDOFFocusFadeOff", 10.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingDOFFocusFadeOff", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_FADE_OFF = propertyType;
    }
    
    /* PostProcessingDOFFocusDistance */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingDOFFocusDistance", 50.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingDOFFocusDistance", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_DISTANCE = propertyType;
    }
    
    /* PostProcessingMotionBlurStrength */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingMotionBlurStrength", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingMotionBlurStrength", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_MOTION_BLUR_STRENGTH = propertyType;
    }
    
    /* PostProcessingRadialBlurAmount */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingRadialBlurAmount", 0.5f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingRadialBlurAmount", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_RADIAL_BLUR_AMOUNT = propertyType;
    }
    
    /* PostProcessingStereoscopicRenderingMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "PostProcessingStereoscopicRenderingMode", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingStereoscopicRenderingMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_RENDERING_MODE = propertyType;
    }
    
    /* PostProcessingStereoscopicEyeDistance */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingStereoscopicEyeDistance", 0.15f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingStereoscopicEyeDistance", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_EYE_DISTANCE = propertyType;
    }
    
    /* PostProcessingStereoscopicFocusDistance */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingStereoscopicFocusDistance", 10.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingStereoscopicFocusDistance", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_FOCUS_DISTANCE = propertyType;
    }
    
    /* PostProcessingVignetteFadeSpeed */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingVignetteFadeSpeed", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingVignetteFadeSpeed", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_FADE_SPEED = propertyType;
    }
    
    /* PostProcessingVignettePower */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingVignettePower", 2.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingVignettePower", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_POWER = propertyType;
    }
    
    /* PostProcessingVignetteXScale */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingVignetteXScale", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingVignetteXScale", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_XSCALE = propertyType;
    }
    
    /* PostProcessingVignetteYScale */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "PostProcessingVignetteYScale", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PostProcessingVignetteYScale", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_YSCALE = propertyType;
    }
    
    /* ShadowPassFarPlane */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ShadowPassFarPlane", 100.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShadowPassFarPlane", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADOW_PASS_FAR_PLANE = propertyType;
    }
    
    /* ShadowPassNearPlane */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ShadowPassNearPlane", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShadowPassNearPlane", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADOW_PASS_NEAR_PLANE = propertyType;
    }
    
    /* ShadowPassShadowCaster */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ShadowPassShadowCaster", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShadowPassShadowCaster", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADOW_PASS_SHADOW_CASTER = propertyType;
    }
    
    /* IsShadowPass */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "IsShadowPass", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "IsShadowPass", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_IS_SHADOW_PASS = propertyType;
    }
    
    /* ShadowPassViewAngle */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ShadowPassViewAngle", 90.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShadowPassViewAngle", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADOW_PASS_VIEW_ANGLE = propertyType;
    }
    
    /* LightEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "LightEnabled", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LightEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LIGHT_ENABLED = propertyType;
    }
    
    /* DirectionalLightColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "DirectionalLightColor", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DirectionalLightColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_COLOR = propertyType;
    }
    
    /* DirectionalLightDirection */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "DirectionalLightDirection", KZU_VECTOR3_TYPE_DIRECTION_FROM_MATRIX4X4, kzcVector3(0.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DirectionalLightDirection", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_DIRECTION = propertyType;
    }
    
    /* DirectionalLightShadowCaster */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "DirectionalLightShadowCaster", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DirectionalLightShadowCaster", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_SHADOW_CASTER = propertyType;
    }
    
    /* PointLightAttenuation */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "PointLightAttenuation", KZU_VECTOR3_TYPE_NORMAL, kzcVector3(1.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PointLightAttenuation", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POINT_LIGHT_ATTENUATION = propertyType;
    }
    
    /* PointLightColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "PointLightColor", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PointLightColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POINT_LIGHT_COLOR = propertyType;
    }
    
    /* PointLightPosition */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "PointLightPosition", KZU_VECTOR3_TYPE_POSITION_FROM_MATRIX4X4, kzcVector3(0.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PointLightPosition", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_POINT_LIGHT_POSITION = propertyType;
    }
    
    /* SpotLightColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "SpotLightColor", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_COLOR = propertyType;
    }
    
    /* SpotLightShadowCaster */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "SpotLightShadowCaster", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightShadowCaster", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_SHADOW_CASTER = propertyType;
    }
    
    /* SpotLightAttenuation */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "SpotLightAttenuation", KZU_VECTOR3_TYPE_NORMAL, kzcVector3(1.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightAttenuation", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_ATTENUATION = propertyType;
    }
    
    /* SpotLightCutoffAngle */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "SpotLightCutoffAngle", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightCutoffAngle", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_CUTOFF_ANGLE = propertyType;
    }
    
    /* SpotLightDirection */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "SpotLightDirection", KZU_VECTOR3_TYPE_DIRECTION_FROM_MATRIX4X4, kzcVector3(0.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightDirection", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_DIRECTION = propertyType;
    }
    
    /* SpotLightExponent */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "SpotLightExponent", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightExponent", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_EXPONENT = propertyType;
    }
    
    /* SpotLightPosition */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "SpotLightPosition", KZU_VECTOR3_TYPE_POSITION_FROM_MATRIX4X4, kzcVector3(0.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SpotLightPosition", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPOT_LIGHT_POSITION = propertyType;
    }
    
    /* GlobalAmbient */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "GlobalAmbient", kzcColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GlobalAmbient", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GLOBAL_AMBIENT = propertyType;
    }
    
    /* FogColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "FogColor", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "FogColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FOG_COLOR = propertyType;
    }
    
    /* FogDensity */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "FogDensity", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "FogDensity", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FOG_DENSITY = propertyType;
    }
    
    /* FogEnd */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "FogEnd", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "FogEnd", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FOG_END = propertyType;
    }
    
    /* FogMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "FogMode", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "FogMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FOG_MODE = propertyType;
    }
    
    /* FogStart */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "FogStart", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "FogStart", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FOG_START = propertyType;
    }
    
    /* Ambient */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "Ambient", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "Ambient", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_AMBIENT = propertyType;
    }
    
    /* BlendIntensity */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "BlendIntensity", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "BlendIntensity", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_BLEND_INTENSITY = propertyType;
    }
    
    /* BlendMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "BlendMode", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "BlendMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_BLEND_MODE = propertyType;
    }
    
    /* Diffuse */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "Diffuse", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "Diffuse", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DIFFUSE = propertyType;
    }
    
    /* Emissive */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "Emissive", kzcColorRGBA(0.0f, 0.0f, 0.0f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "Emissive", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_EMISSIVE = propertyType;
    }
    
    /* Texture */
    {
        struct KzuTexturePropertyType* propertyTypeTexture;
        struct KzuPropertyType* propertyType;
        result = kzuTexturePropertyTypeCreate(memoryManager, "Texture", KZU_TEXTURE_OPERATION_MODULATE, 0, KZ_NULL, KZ_NULL, &propertyTypeTexture);
        kzsErrorForward(result);
        propertyType = kzuTexturePropertyTypeToPropertyType(propertyTypeTexture);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "Texture", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TEXTURE = propertyType;
    }
    
    /* ColorIntensity */
    {
        struct KzuVector3PropertyType* propertyTypeVector3;
        struct KzuPropertyType* propertyType;
        result = kzuVector3PropertyTypeCreate(memoryManager, "ColorIntensity", KZU_VECTOR3_TYPE_NORMAL, kzcVector3(0.0f, 0.0f, 0.0f), &propertyTypeVector3);
        kzsErrorForward(result);
        propertyType = kzuVector3PropertyTypeToPropertyType(propertyTypeVector3);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "ColorIntensity", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_COLOR_INTENSITY = propertyType;
    }
    
    /* SpecularColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "SpecularColor", kzcColorRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "SpecularColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPECULAR_COLOR = propertyType;
    }
    
    /* SpecularExponent */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "SpecularExponent", 50.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "SpecularExponent", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SPECULAR_EXPONENT = propertyType;
    }
    
    /* ChangeRenderPassCameraCamera */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ChangeRenderPassCameraCamera", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ChangeRenderPassCameraCamera", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_RENDER_PASS_CAMERA_CAMERA = propertyType;
    }
    
    /* ChangeRenderPassCameraRenderPass */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ChangeRenderPassCameraRenderPass", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ChangeRenderPassCameraRenderPass", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_RENDER_PASS_CAMERA_RENDER_PASS = propertyType;
    }
    
    /* ChangeComposerComposer */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ChangeComposerComposer", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ChangeComposerComposer", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_COMPOSER_COMPOSER = propertyType;
    }
    
    /* ChangeComposerScene */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ChangeComposerScene", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ChangeComposerScene", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_COMPOSER_SCENE = propertyType;
    }
    
    /* RenderPassActionRenderPass */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "RenderPassActionRenderPass", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "RenderPassActionRenderPass", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_RENDER_PASS_ACTION_RENDER_PASS = propertyType;
    }
    
    /* RenderPassActionEnabled */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "RenderPassActionEnabled", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "RenderPassActionEnabled", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_RENDER_PASS_ACTION_ENABLED = propertyType;
    }
    
    /* Change2DPickingCameraScene */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "Change2DPickingCameraScene", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "Change2DPickingCameraScene", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_2DPICKING_CAMERA_SCENE = propertyType;
    }
    
    /* Change2DPickingCameraCamera */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "Change2DPickingCameraCamera", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "Change2DPickingCameraCamera", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_2DPICKING_CAMERA_CAMERA = propertyType;
    }
    
    /* ChangeSceneCameraScene */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ChangeSceneCameraScene", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ChangeSceneCameraScene", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_SCENE_CAMERA_SCENE = propertyType;
    }
    
    /* ChangeSceneCameraCamera */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ChangeSceneCameraCamera", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ChangeSceneCameraCamera", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CHANGE_SCENE_CAMERA_CAMERA = propertyType;
    }
    
    /* AnimationPlaybackMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "AnimationPlaybackMode", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "AnimationPlaybackMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_ANIMATION_PLAYBACK_MODE = propertyType;
    }
    
    /* AnimationDurationScale */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "AnimationDurationScale", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "AnimationDurationScale", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_ANIMATION_DURATION_SCALE = propertyType;
    }
    
    /* AnimationRepeatCount */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "AnimationRepeatCount", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "AnimationRepeatCount", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_ANIMATION_REPEAT_COUNT = propertyType;
    }
    
    /* PlayAnimationInputPropertyHostItem */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "PlayAnimationInputPropertyHostItem", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlayAnimationInputPropertyHostItem", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_HOST_ITEM = propertyType;
    }
    
    /* PlayAnimationInputPropertyName */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "PlayAnimationInputPropertyName", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlayAnimationInputPropertyName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_NAME = propertyType;
    }
    
    /* PlayAnimationInputPropertyAttribute */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "PlayAnimationInputPropertyAttribute", 2, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlayAnimationInputPropertyAttribute", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_ATTRIBUTE = propertyType;
    }
    
    /* AnimationItemName */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "AnimationItemName", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "AnimationItemName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_ANIMATION_ITEM_NAME = propertyType;
    }
    
    /* PlayAnimationInputType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "PlayAnimationInputType", 0/* TODO: Specify default value in XML or remove from engine */, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlayAnimationInputType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_TYPE = propertyType;
    }
    
    /* StartTransitionReference */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "StartTransitionReference", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StartTransitionReference", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_START_TRANSITION_REFERENCE = propertyType;
    }
    
    /* StartTransitionTargetScene */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "StartTransitionTargetScene", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StartTransitionTargetScene", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_START_TRANSITION_TARGET_SCENE = propertyType;
    }
    
    /* StartTransitionTargetCamera */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "StartTransitionTargetCamera", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StartTransitionTargetCamera", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_START_TRANSITION_TARGET_CAMERA = propertyType;
    }
    
    /* StartTransitionDuration */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "StartTransitionDuration", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StartTransitionDuration", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_START_TRANSITION_DURATION = propertyType;
    }
    
    /* SetPropertyTargetItem */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "SetPropertyTargetItem", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SetPropertyTargetItem", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_ITEM = propertyType;
    }
    
    /* SetPropertyTargetPropertyName */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "SetPropertyTargetPropertyName", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SetPropertyTargetPropertyName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_PROPERTY_NAME = propertyType;
    }
    
    /* SetPropertyTargetPropertyAttribute */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "SetPropertyTargetPropertyAttribute", 2, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SetPropertyTargetPropertyAttribute", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_PROPERTY_ATTRIBUTE = propertyType;
    }
    
    /* SetPropertyBooleanValue */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "SetPropertyBooleanValue", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SetPropertyBooleanValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SET_PROPERTY_BOOLEAN_VALUE = propertyType;
    }
    
    /* SetPropertyTextureValue */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "SetPropertyTextureValue", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SetPropertyTextureValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SET_PROPERTY_TEXTURE_VALUE = propertyType;
    }
    
    /* SetTextureTargetItem */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "SetTextureTargetItem", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SetTextureTargetItem", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SET_TEXTURE_TARGET_ITEM = propertyType;
    }
    
    /* AnimationItemToReset */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "AnimationItemToReset", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "AnimationItemToReset", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_ANIMATION_ITEM_TO_RESET = propertyType;
    }
    
    /* VisibleAmountInParent */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "VisibleAmountInParent", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "VisibleAmountInParent", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VISIBLE_AMOUNT_IN_PARENT = propertyType;
    }
    
    /* PlaylistAnimationItemName */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "PlaylistAnimationItemName", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlaylistAnimationItemName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAYLIST_ANIMATION_ITEM_NAME = propertyType;
    }
    
    /* ClipChildren */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "ClipChildren", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ClipChildren", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CLIP_CHILDREN = propertyType;
    }
    
    /* ComponentPressed */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "ComponentPressed", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ComponentPressed", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_COMPONENT_PRESSED = propertyType;
    }
    
    /* ContentContainer */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "ContentContainer", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ContentContainer", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CONTENT_CONTAINER = propertyType;
    }
    
    /* CullMode */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "CullMode", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CullMode", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CULL_MODE = propertyType;
    }
    
    /* CustomNodeTypeID */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "CustomNodeTypeID", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "CustomNodeTypeID", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_CUSTOM_NODE_TYPE_ID = propertyType;
    }
    
    /* StackLayoutDirection */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "StackLayoutDirection", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StackLayoutDirection", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STACK_LAYOUT_DIRECTION = propertyType;
    }
    
    /* SliderDirection */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "SliderDirection", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SliderDirection", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SLIDER_DIRECTION = propertyType;
    }
    
    /* SliderExpandDirection */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "SliderExpandDirection", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SliderExpandDirection", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SLIDER_EXPAND_DIRECTION = propertyType;
    }
    
    /* FaceToCamera */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "FaceToCamera", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "FaceToCamera", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FACE_TO_CAMERA = propertyType;
    }
    
    /* ScriptFunctionName */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "ScriptFunctionName", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ScriptFunctionName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SCRIPT_FUNCTION_NAME = propertyType;
    }
    
    /* SliderInvertDirection */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "SliderInvertDirection", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SliderInvertDirection", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SLIDER_INVERT_DIRECTION = propertyType;
    }
    
    /* ToggleButtonState */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "ToggleButtonState", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ToggleButtonState", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE = propertyType;
    }
    
    /* PlaylistItems */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "PlaylistItems", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlaylistItems", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAYLIST_ITEMS = propertyType;
    }
    
    /* LookAt */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "LookAt", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LookAt", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LOOK_AT = propertyType;
    }
    
    /* PlaylistLooping */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "PlaylistLooping", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlaylistLooping", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAYLIST_LOOPING = propertyType;
    }
    
    /* Pickable */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "Pickable", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "Pickable", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PICKABLE = propertyType;
    }
    
    /* Pickable2D */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "Pickable2D", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "Pickable2D", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PICKABLE_2D = propertyType;
    }
    
    /* PlaylistPlayType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "PlaylistPlayType", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlaylistPlayType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAYLIST_PLAY_TYPE = propertyType;
    }
    
    /* StackLayoutReversed */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "StackLayoutReversed", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StackLayoutReversed", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STACK_LAYOUT_REVERSED = propertyType;
    }
    
    /* SceneName */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "SceneName", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SceneName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SCENE_NAME = propertyType;
    }
    
    /* PlaylistIndex */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "PlaylistIndex", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "PlaylistIndex", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_PLAYLIST_INDEX = propertyType;
    }
    
    /* IntValue */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "IntValue", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "IntValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_INT_VALUE = propertyType;
    }
    
    /* Visible */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "Visible", KZ_TRUE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_TRUE);
        result = kzcHashMapPut(g_fixedProperties, "Visible", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VISIBLE = propertyType;
    }
    
    /* FloatValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "FloatValue", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "FloatValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_FLOAT_VALUE = propertyType;
    }
    
    /* DropdownSelectedItem */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "DropdownSelectedItem", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DropdownSelectedItem", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM = propertyType;
    }
    
    /* DropdownOpenState */
    {
        struct KzuBoolPropertyType* propertyTypeBool;
        struct KzuPropertyType* propertyType;
        result = kzuBoolPropertyTypeCreate(memoryManager, "DropdownOpenState", KZ_FALSE, &propertyTypeBool);
        kzsErrorForward(result);
        propertyType = kzuBoolPropertyTypeToPropertyType(propertyTypeBool);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DropdownOpenState", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE = propertyType;
    }
    
    /* GridLayoutColumnDefinitions */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "GridLayoutColumnDefinitions", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutColumnDefinitions", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_DEFINITIONS = propertyType;
    }
    
    /* GridLayoutRowDefinitions */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "GridLayoutRowDefinitions", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutRowDefinitions", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_DEFINITIONS = propertyType;
    }
    
    /* GridLayoutColumn */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "GridLayoutColumn", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutColumn", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN = propertyType;
    }
    
    /* GridLayoutColumnSpan */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "GridLayoutColumnSpan", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutColumnSpan", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN = propertyType;
    }
    
    /* GridLayoutRow */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "GridLayoutRow", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutRow", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW = propertyType;
    }
    
    /* GridLayoutRowSpan */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "GridLayoutRowSpan", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutRowSpan", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN = propertyType;
    }
    
    /* Trajectory */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "Trajectory", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "Trajectory", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TRAJECTORY = propertyType;
    }
    
    /* TrajectoryLayoutStartValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "TrajectoryLayoutStartValue", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "TrajectoryLayoutStartValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_START_VALUE = propertyType;
    }
    
    /* TrajectoryLayoutEndValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "TrajectoryLayoutEndValue", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "TrajectoryLayoutEndValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_END_VALUE = propertyType;
    }
    
    /* TrajectoryLayoutOffset */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "TrajectoryLayoutOffset", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "TrajectoryLayoutOffset", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_OFFSET = propertyType;
    }
    
    /* LayoutWidth */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "LayoutWidth", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutWidth", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_WIDTH = propertyType;
    }
    
    /* LayoutHeight */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "LayoutHeight", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutHeight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_HEIGHT = propertyType;
    }
    
    /* LayoutDepth */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "LayoutDepth", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutDepth", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_DEPTH = propertyType;
    }
    
    /* LayoutHorizontalAlignment */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LayoutHorizontalAlignment", 2, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutHorizontalAlignment", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_ALIGNMENT = propertyType;
    }
    
    /* LayoutVerticalAlignment */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LayoutVerticalAlignment", 2, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutVerticalAlignment", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_ALIGNMENT = propertyType;
    }
    
    /* LayoutDepthAlignment */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LayoutDepthAlignment", 2, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutDepthAlignment", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_DEPTH_ALIGNMENT = propertyType;
    }
    
    /* LayoutHorizontalMargin */
    {
        struct KzuVector2PropertyType* propertyTypeVector2;
        struct KzuPropertyType* propertyType;
        result = kzuVector2PropertyTypeCreate(memoryManager, "LayoutHorizontalMargin", kzcVector2(0.0f, 0.0f), &propertyTypeVector2);
        kzsErrorForward(result);
        propertyType = kzuVector2PropertyTypeToPropertyType(propertyTypeVector2);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutHorizontalMargin", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_MARGIN = propertyType;
    }
    
    /* LayoutVerticalMargin */
    {
        struct KzuVector2PropertyType* propertyTypeVector2;
        struct KzuPropertyType* propertyType;
        result = kzuVector2PropertyTypeCreate(memoryManager, "LayoutVerticalMargin", kzcVector2(0.0f, 0.0f), &propertyTypeVector2);
        kzsErrorForward(result);
        propertyType = kzuVector2PropertyTypeToPropertyType(propertyTypeVector2);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutVerticalMargin", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_MARGIN = propertyType;
    }
    
    /* LayoutDepthMargin */
    {
        struct KzuVector2PropertyType* propertyTypeVector2;
        struct KzuPropertyType* propertyType;
        result = kzuVector2PropertyTypeCreate(memoryManager, "LayoutDepthMargin", kzcVector2(0.0f, 0.0f), &propertyTypeVector2);
        kzsErrorForward(result);
        propertyType = kzuVector2PropertyTypeToPropertyType(propertyTypeVector2);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutDepthMargin", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_DEPTH_MARGIN = propertyType;
    }
    
    /* LayoutWidthType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LayoutWidthType", 0 /* TODO: Specify default value in XML */, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutWidthType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_WIDTH_TYPE = propertyType;
    }
    
    /* LayoutHeightType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LayoutHeightType", 0 /* TODO: Specify default value in XML */, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutHeightType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_HEIGHT_TYPE = propertyType;
    }
    
    /* LayoutDepthType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LayoutDepthType", 0 /* TODO: Specify default value in XML */, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LayoutDepthType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LAYOUT_DEPTH_TYPE = propertyType;
    }
    
    /* ButtonLongReleaseInterval */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ButtonLongReleaseInterval", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ButtonLongReleaseInterval", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_BUTTON_LONG_RELEASE_INTERVAL = propertyType;
    }
    
    /* ButtonRepeatInterval */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ButtonRepeatInterval", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ButtonRepeatInterval", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_BUTTON_REPEAT_INTERVAL = propertyType;
    }
    
    /* LabelColor */
    {
        struct KzuColorPropertyType* propertyTypeColor;
        struct KzuPropertyType* propertyType;
        result = kzuColorPropertyTypeCreate(memoryManager, "LabelColor", kzcColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), &propertyTypeColor);
        kzsErrorForward(result);
        propertyType = kzuColorPropertyTypeToPropertyType(propertyTypeColor);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelColor", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_COLOR = propertyType;
    }
    
    /* LabelFont */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "LabelFont", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelFont", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_FONT = propertyType;
    }
    
    /* LabelFontSize */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "LabelFontSize", 36.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelFontSize", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_FONT_SIZE = propertyType;
    }
    
    /* LabelLineSpacing */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "LabelLineSpacing", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelLineSpacing", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_LINE_SPACING = propertyType;
    }
    
    /* LabelMaximumCharactersPerLine */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LabelMaximumCharactersPerLine", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelMaximumCharactersPerLine", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_MAXIMUM_CHARACTERS_PER_LINE = propertyType;
    }
    
    /* LabelMaximumHeight */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LabelMaximumHeight", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelMaximumHeight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_MAXIMUM_HEIGHT = propertyType;
    }
    
    /* LabelMaximumLines */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LabelMaximumLines", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelMaximumLines", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_MAXIMUM_LINES = propertyType;
    }
    
    /* LabelMaximumWidth */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LabelMaximumWidth", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelMaximumWidth", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_MAXIMUM_WIDTH = propertyType;
    }
    
    /* LabelTerminator */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "LabelTerminator", "...", &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelTerminator", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_TERMINATOR = propertyType;
    }
    
    /* LabelText */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "LabelText", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelText", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_TEXT = propertyType;
    }
    
    /* LabelHorizontalAlignment */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LabelHorizontalAlignment", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelHorizontalAlignment", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_HORIZONTAL_ALIGNMENT = propertyType;
    }
    
    /* LabelVerticalAlignment */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "LabelVerticalAlignment", 0, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "LabelVerticalAlignment", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_LABEL_VERTICAL_ALIGNMENT = propertyType;
    }
    
    /* SliderValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "SliderValue", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SliderValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SLIDER_VALUE = propertyType;
    }
    
    /* SliderMinimumValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "SliderMinimumValue", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SliderMinimumValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SLIDER_MINIMUM_VALUE = propertyType;
    }
    
    /* SliderMaximumValue */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "SliderMaximumValue", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "SliderMaximumValue", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SLIDER_MAXIMUM_VALUE = propertyType;
    }
    
    /* DropdownNewSelectedItem */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "DropdownNewSelectedItem", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DropdownNewSelectedItem", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DROPDOWN_NEW_SELECTED_ITEM = propertyType;
    }
    
    /* DropdownOldSelectedItem */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "DropdownOldSelectedItem", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "DropdownOldSelectedItem", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_DROPDOWN_OLD_SELECTED_ITEM = propertyType;
    }
    
    /* ShaderTransitionMaterial */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "ShaderTransitionMaterial", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShaderTransitionMaterial", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADER_TRANSITION_MATERIAL = propertyType;
    }
    
    /* ShaderTransitionRenderTargetWidth */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "ShaderTransitionRenderTargetWidth", 512, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShaderTransitionRenderTargetWidth", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_WIDTH = propertyType;
    }
    
    /* ShaderTransitionRenderTargetHeight */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "ShaderTransitionRenderTargetHeight", 512, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ShaderTransitionRenderTargetHeight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_HEIGHT = propertyType;
    }
    
    /* ViewportX */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ViewportX", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ViewportX", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VIEWPORT_X = propertyType;
    }
    
    /* ViewportY */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ViewportY", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ViewportY", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VIEWPORT_Y = propertyType;
    }
    
    /* ViewportWidth */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ViewportWidth", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ViewportWidth", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VIEWPORT_WIDTH = propertyType;
    }
    
    /* ViewportHeight */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ViewportHeight", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ViewportHeight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VIEWPORT_HEIGHT = propertyType;
    }
    
    /* ViewportType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "ViewportType", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ViewportType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_VIEWPORT_TYPE = propertyType;
    }
    
    /* StartupScene */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "StartupScene", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "StartupScene", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_STARTUP_SCENE = propertyType;
    }
    
    /* WindowMetricsType */
    {
        struct KzuIntPropertyType* propertyTypeInt;
        struct KzuPropertyType* propertyType;
        result = kzuIntPropertyTypeCreate(memoryManager, "WindowMetricsType", 1, &propertyTypeInt);
        kzsErrorForward(result);
        propertyType = kzuIntPropertyTypeToPropertyType(propertyTypeInt);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "WindowMetricsType", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_WINDOW_METRICS_TYPE = propertyType;
    }
    
    /* WindowX */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "WindowX", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "WindowX", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_WINDOW_X = propertyType;
    }
    
    /* WindowY */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "WindowY", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "WindowY", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_WINDOW_Y = propertyType;
    }
    
    /* WindowWidth */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "WindowWidth", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "WindowWidth", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_WINDOW_WIDTH = propertyType;
    }
    
    /* WindowHeight */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "WindowHeight", 1.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "WindowHeight", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_WINDOW_HEIGHT = propertyType;
    }
    
    /* WindowAspectRatio */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "WindowAspectRatio", 1.6666f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "WindowAspectRatio", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_WINDOW_ASPECT_RATIO = propertyType;
    }
    
    /* MessageArgumentTargetObject */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "MessageArgumentTargetObject", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "MessageArgumentTargetObject", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT = propertyType;
    }
    
    /* MessageArgumentTargetObjectName */
    {
        struct KzuStringPropertyType* propertyTypeString;
        struct KzuPropertyType* propertyType;
        result = kzuStringPropertyTypeCreate(memoryManager, "MessageArgumentTargetObjectName", KZ_NULL, &propertyTypeString);
        kzsErrorForward(result);
        propertyType = kzuStringPropertyTypeToPropertyType(propertyTypeString);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "MessageArgumentTargetObjectName", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT_NAME = propertyType;
    }
    
    /* TextLabelObject */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "TextLabelObject", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "TextLabelObject", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT = propertyType;
    }
    
    /* GridLayoutData */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "GridLayoutData", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "GridLayoutData", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_GRID_LAYOUT_DATA = propertyType;
    }
    
    /* TargetComponentLocation */
    {
        struct KzuVoidPropertyType* propertyTypeVoid;
        struct KzuPropertyType* propertyType;
        result = kzuVoidPropertyTypeCreate(memoryManager, "TargetComponentLocation", KZ_NULL, &propertyTypeVoid);
        kzsErrorForward(result);
        propertyType = kzuVoidPropertyTypeToPropertyType(propertyTypeVoid);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "TargetComponentLocation", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_TARGET_COMPONENT_LOCATION = propertyType;
    }
    
    /* ButtonPressDuration */
    {
        struct KzuFloatPropertyType* propertyTypeFloat;
        struct KzuPropertyType* propertyType;
        result = kzuFloatPropertyTypeCreate(memoryManager, "ButtonPressDuration", 0.0f, &propertyTypeFloat);
        kzsErrorForward(result);
        propertyType = kzuFloatPropertyTypeToPropertyType(propertyTypeFloat);
        kzuPropertyTypeSetInherited(propertyType, KZ_FALSE);
        result = kzcHashMapPut(g_fixedProperties, "ButtonPressDuration", propertyType);
        kzsErrorForward(result);
        KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION = propertyType;
    }
    
    result = kzuFixedPropertiesCreateLightTypes_internal(memoryManager); /* TODO: Remove when light property types are removed */
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzsError kzuFixedPropertiesDelete(void)
{
    kzsError result;
    struct KzcHashMapIterator it;

    kzsAssert(kzcIsValidPointer(g_fixedProperties));

    it = kzcHashMapGetIterator(g_fixedProperties);
    while (kzcHashMapIterate(it))
    {
        struct KzuPropertyType* propertyType = (struct KzuPropertyType*)kzcHashMapIteratorGetValue(it);
        result = kzuPropertyTypeDelete(propertyType);
        kzsErrorForward(result);
    }
    
    result = kzcHashMapDelete(g_fixedProperties);
    kzsErrorForward(result);
    
    g_fixedProperties = KZ_NULL;
    
    kzsSuccess();
}

struct KzuPropertyType* kzuFixedPropertiesFindPropertyType(kzString name)
{
    struct KzuPropertyType* propertyType;
    
    kzsAssert(kzcIsValidPointer(g_fixedProperties));
    
    if (!kzcHashMapGet(g_fixedProperties, name, (void**)&propertyType))
    {
        propertyType = KZ_NULL;
    }
    
    return propertyType;
}
