/**
 * \file
 * Fixed property types.
 * 
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
#ifndef KZU_FIXED_PROPERTIES_H
#define KZU_FIXED_PROPERTIES_H


#include <system/kzs_types.h>
#include <system/debug/kzs_error.h>


/* Forward declarations */
struct KzuPropertyType;
struct KzuFixedPropertyTypes;
struct KzcMemoryManager;


/*lint --e{956} Suppress warning of global non-const variables. */

/** Fixed property type CameraAspectRatio. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ASPECT_RATIO;

/** Fixed property type CameraDisableAspectRatio. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_DISABLE_ASPECT_RATIO;

/** Fixed property type CameraZNear. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ZNEAR;

/** Fixed property type CameraZFar. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ZFAR;

/** Fixed property type CameraProjectionType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE;

/** Fixed property type CameraOrthogonalCoordinateSystemType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE;

/** Fixed property type CameraOrthogonalPlaneHeight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_PLANE_HEIGHT;

/** Fixed property type CameraFovType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_FOV_TYPE;

/** Fixed property type CameraFov. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_FOV;

/** Fixed property type CameraOrientationAngle. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CAMERA_ORIENTATION_ANGLE;

/** Fixed property type SceneStartupScriptFunctionName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SCENE_STARTUP_SCRIPT_FUNCTION_NAME;

/** Fixed property type Transformation. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRANSFORMATION;

/** Fixed property type FrustumCullingOn. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FRUSTUM_CULLING_ON;

/** Fixed property type RenderPassIs2d. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_RENDER_PASS_IS_2D;

/** Fixed property type ColorBufferClearColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_COLOR;

/** Fixed property type ColorBufferClearEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_BUFFER_CLEAR_ENABLED;

/** Fixed property type ColorWriteMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_WRITE_MODE;

/** Fixed property type DepthBufferClearEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_ENABLED;

/** Fixed property type DepthBufferClearValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_CLEAR_VALUE;

/** Fixed property type DepthBufferTestEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_TEST_ENABLED;

/** Fixed property type DepthBufferWriteEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DEPTH_BUFFER_WRITE_ENABLED;

/** Fixed property type StencilBufferClearEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_ENABLED;

/** Fixed property type StencilBufferClearValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_BUFFER_CLEAR_VALUE;

/** Fixed property type StencilFunction. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_FUNCTION;

/** Fixed property type StencilWriteMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_WRITE_MODE;

/** Fixed property type StencilBufferTestEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STENCIL_BUFFER_TEST_ENABLED;

/** Fixed property type PostProcessingBloomIntensity. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_INTENSITY;

/** Fixed property type PostProcessingBloomSpread. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_BLOOM_SPREAD;

/** Fixed property type PostProcessingBoxBlurSpread. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_BOX_BLUR_SPREAD;

/** Fixed property type PostProcessingColorAdjustBrightness. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_BRIGHTNESS;

/** Fixed property type PostProcessingColorAdjustContrast. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_CONTRAST;

/** Fixed property type PostProcessingColorAdjustGammaCorrect. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_GAMMA_CORRECT;

/** Fixed property type PostProcessingColorAdjustHue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_HUE;

/** Fixed property type PostProcessingColorAdjustLightness. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_LIGHTNESS;

/** Fixed property type PostProcessingColorAdjustSaturation. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_COLOR_ADJUST_SATURATION;

/** Fixed property type PostProcessingSelectedComposer. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_SELECTED_COMPOSER;

/** Fixed property type PostProcessingDOFFocusAreaLength. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_AREA_LENGTH;

/** Fixed property type PostProcessingDOFBlurSpread. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFBLUR_SPREAD;

/** Fixed property type PostProcessingDOFFocusFadeOff. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_FADE_OFF;

/** Fixed property type PostProcessingDOFFocusDistance. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_DOFFOCUS_DISTANCE;

/** Fixed property type PostProcessingMotionBlurStrength. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_MOTION_BLUR_STRENGTH;

/** Fixed property type PostProcessingRadialBlurAmount. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_RADIAL_BLUR_AMOUNT;

/** Fixed property type PostProcessingStereoscopicRenderingMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_RENDERING_MODE;

/** Fixed property type PostProcessingStereoscopicEyeDistance. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_EYE_DISTANCE;

/** Fixed property type PostProcessingStereoscopicFocusDistance. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_STEREOSCOPIC_FOCUS_DISTANCE;

/** Fixed property type PostProcessingVignetteFadeSpeed. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_FADE_SPEED;

/** Fixed property type PostProcessingVignettePower. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_POWER;

/** Fixed property type PostProcessingVignetteXScale. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_XSCALE;

/** Fixed property type PostProcessingVignetteYScale. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POST_PROCESSING_VIGNETTE_YSCALE;

/** Fixed property type ShadowPassFarPlane. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_FAR_PLANE;

/** Fixed property type ShadowPassNearPlane. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_NEAR_PLANE;

/** Fixed property type ShadowPassShadowCaster. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_SHADOW_CASTER;

/** Fixed property type IsShadowPass. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_IS_SHADOW_PASS;

/** Fixed property type ShadowPassViewAngle. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADOW_PASS_VIEW_ANGLE;

/** Fixed property type LightEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LIGHT_ENABLED;

/** Fixed property type DirectionalLightColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_COLOR;

/** Fixed property type DirectionalLightDirection. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_DIRECTION;

/** Fixed property type DirectionalLightShadowCaster. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT_SHADOW_CASTER;

/** Fixed property type PointLightAttenuation. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT_ATTENUATION;

/** Fixed property type PointLightColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT_COLOR;

/** Fixed property type PointLightPosition. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT_POSITION;

/** Fixed property type SpotLightColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_COLOR;

/** Fixed property type SpotLightShadowCaster. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_SHADOW_CASTER;

/** Fixed property type SpotLightAttenuation. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_ATTENUATION;

/** Fixed property type SpotLightCutoffAngle. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_CUTOFF_ANGLE;

/** Fixed property type SpotLightDirection. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_DIRECTION;

/** Fixed property type SpotLightExponent. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_EXPONENT;

/** Fixed property type SpotLightPosition. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT_POSITION;

/** Fixed property type GlobalAmbient. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GLOBAL_AMBIENT;

/** Fixed property type FogColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_COLOR;

/** Fixed property type FogDensity. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_DENSITY;

/** Fixed property type FogEnd. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_END;

/** Fixed property type FogMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_MODE;

/** Fixed property type FogStart. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FOG_START;

/** Fixed property type Ambient. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_AMBIENT;

/** Fixed property type BlendIntensity. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_BLEND_INTENSITY;

/** Fixed property type BlendMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_BLEND_MODE;

/** Fixed property type Diffuse. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIFFUSE;

/** Fixed property type Emissive. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_EMISSIVE;

/** Fixed property type Texture. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TEXTURE;

/** Fixed property type ColorIntensity. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_COLOR_INTENSITY;

/** Fixed property type SpecularColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPECULAR_COLOR;

/** Fixed property type SpecularExponent. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPECULAR_EXPONENT;

/** Fixed property type ChangeRenderPassCameraCamera. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_RENDER_PASS_CAMERA_CAMERA;

/** Fixed property type ChangeRenderPassCameraRenderPass. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_RENDER_PASS_CAMERA_RENDER_PASS;

/** Fixed property type ChangeComposerComposer. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_COMPOSER_COMPOSER;

/** Fixed property type ChangeComposerScene. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_COMPOSER_SCENE;

/** Fixed property type RenderPassActionRenderPass. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_RENDER_PASS_ACTION_RENDER_PASS;

/** Fixed property type RenderPassActionEnabled. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_RENDER_PASS_ACTION_ENABLED;

/** Fixed property type Change2DPickingCameraScene. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_2DPICKING_CAMERA_SCENE;

/** Fixed property type Change2DPickingCameraCamera. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_2DPICKING_CAMERA_CAMERA;

/** Fixed property type ChangeSceneCameraScene. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_SCENE_CAMERA_SCENE;

/** Fixed property type ChangeSceneCameraCamera. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CHANGE_SCENE_CAMERA_CAMERA;

/** Fixed property type AnimationPlaybackMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_PLAYBACK_MODE;

/** Fixed property type AnimationDurationScale. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_DURATION_SCALE;

/** Fixed property type AnimationRepeatCount. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_REPEAT_COUNT;

/** Fixed property type PlayAnimationInputPropertyHostItem. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_HOST_ITEM;

/** Fixed property type PlayAnimationInputPropertyName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_NAME;

/** Fixed property type PlayAnimationInputPropertyAttribute. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_PROPERTY_ATTRIBUTE;

/** Fixed property type AnimationItemName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_ITEM_NAME;

/** Fixed property type PlayAnimationInputType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAY_ANIMATION_INPUT_TYPE;

/** Fixed property type StartTransitionReference. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_REFERENCE;

/** Fixed property type StartTransitionTargetScene. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_TARGET_SCENE;

/** Fixed property type StartTransitionTargetCamera. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_TARGET_CAMERA;

/** Fixed property type StartTransitionDuration. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_START_TRANSITION_DURATION;

/** Fixed property type SetPropertyTargetItem. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_ITEM;

/** Fixed property type SetPropertyTargetPropertyName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_PROPERTY_NAME;

/** Fixed property type SetPropertyTargetPropertyAttribute. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TARGET_PROPERTY_ATTRIBUTE;

/** Fixed property type SetPropertyBooleanValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_BOOLEAN_VALUE;

/** Fixed property type SetPropertyTextureValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_PROPERTY_TEXTURE_VALUE;

/** Fixed property type SetTextureTargetItem. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SET_TEXTURE_TARGET_ITEM;

/** Fixed property type AnimationItemToReset. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_ANIMATION_ITEM_TO_RESET;

/** Fixed property type VisibleAmountInParent. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VISIBLE_AMOUNT_IN_PARENT;

/** Fixed property type PlaylistAnimationItemName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_ANIMATION_ITEM_NAME;

/** Fixed property type ClipChildren. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CLIP_CHILDREN;

/** Fixed property type ComponentPressed. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_COMPONENT_PRESSED;

/** Fixed property type ContentContainer. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CONTENT_CONTAINER;

/** Fixed property type CullMode. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CULL_MODE;

/** Fixed property type CustomNodeTypeID. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_CUSTOM_NODE_TYPE_ID;

/** Fixed property type StackLayoutDirection. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STACK_LAYOUT_DIRECTION;

/** Fixed property type SliderDirection. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_DIRECTION;

/** Fixed property type SliderExpandDirection. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_EXPAND_DIRECTION;

/** Fixed property type FaceToCamera. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FACE_TO_CAMERA;

/** Fixed property type ScriptFunctionName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SCRIPT_FUNCTION_NAME;

/** Fixed property type SliderInvertDirection. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_INVERT_DIRECTION;

/** Fixed property type ToggleButtonState. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TOGGLE_BUTTON_STATE;

/** Fixed property type PlaylistItems. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_ITEMS;

/** Fixed property type LookAt. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LOOK_AT;

/** Fixed property type PlaylistLooping. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_LOOPING;

/** Fixed property type Pickable. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PICKABLE;

/** Fixed property type Pickable2D. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PICKABLE_2D;

/** Fixed property type PlaylistPlayType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_PLAY_TYPE;

/** Fixed property type StackLayoutReversed. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STACK_LAYOUT_REVERSED;

/** Fixed property type SceneName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SCENE_NAME;

/** Fixed property type PlaylistIndex. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_PLAYLIST_INDEX;

/** Fixed property type IntValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_INT_VALUE;

/** Fixed property type Visible. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VISIBLE;

/** Fixed property type FloatValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_FLOAT_VALUE;

/** Fixed property type DropdownSelectedItem. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_SELECTED_ITEM;

/** Fixed property type DropdownOpenState. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_OPEN_STATE;

/** Fixed property type GridLayoutColumnDefinitions. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_DEFINITIONS;

/** Fixed property type GridLayoutRowDefinitions. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_DEFINITIONS;

/** Fixed property type GridLayoutColumn. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN;

/** Fixed property type GridLayoutColumnSpan. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_COLUMN_SPAN;

/** Fixed property type GridLayoutRow. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW;

/** Fixed property type GridLayoutRowSpan. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_ROW_SPAN;

/** Fixed property type Trajectory. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY;

/** Fixed property type TrajectoryLayoutStartValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_START_VALUE;

/** Fixed property type TrajectoryLayoutEndValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_END_VALUE;

/** Fixed property type TrajectoryLayoutOffset. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TRAJECTORY_LAYOUT_OFFSET;

/** Fixed property type LayoutWidth. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_WIDTH;

/** Fixed property type LayoutHeight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HEIGHT;

/** Fixed property type LayoutDepth. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH;

/** Fixed property type LayoutHorizontalAlignment. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_ALIGNMENT;

/** Fixed property type LayoutVerticalAlignment. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_ALIGNMENT;

/** Fixed property type LayoutDepthAlignment. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH_ALIGNMENT;

/** Fixed property type LayoutHorizontalMargin. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HORIZONTAL_MARGIN;

/** Fixed property type LayoutVerticalMargin. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_VERTICAL_MARGIN;

/** Fixed property type LayoutDepthMargin. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH_MARGIN;

/** Fixed property type LayoutWidthType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_WIDTH_TYPE;

/** Fixed property type LayoutHeightType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_HEIGHT_TYPE;

/** Fixed property type LayoutDepthType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LAYOUT_DEPTH_TYPE;

/** Fixed property type ButtonLongReleaseInterval. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_BUTTON_LONG_RELEASE_INTERVAL;

/** Fixed property type ButtonRepeatInterval. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_BUTTON_REPEAT_INTERVAL;

/** Fixed property type LabelColor. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_COLOR;

/** Fixed property type LabelFont. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_FONT;

/** Fixed property type LabelFontSize. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_FONT_SIZE;

/** Fixed property type LabelLineSpacing. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_LINE_SPACING;

/** Fixed property type LabelMaximumCharactersPerLine. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_CHARACTERS_PER_LINE;

/** Fixed property type LabelMaximumHeight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_HEIGHT;

/** Fixed property type LabelMaximumLines. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_LINES;

/** Fixed property type LabelMaximumWidth. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_MAXIMUM_WIDTH;

/** Fixed property type LabelTerminator. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_TERMINATOR;

/** Fixed property type LabelText. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_TEXT;

/** Fixed property type LabelHorizontalAlignment. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_HORIZONTAL_ALIGNMENT;

/** Fixed property type LabelVerticalAlignment. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_LABEL_VERTICAL_ALIGNMENT;

/** Fixed property type SliderValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_VALUE;

/** Fixed property type SliderMinimumValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_MINIMUM_VALUE;

/** Fixed property type SliderMaximumValue. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SLIDER_MAXIMUM_VALUE;

/** Fixed property type DropdownNewSelectedItem. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_NEW_SELECTED_ITEM;

/** Fixed property type DropdownOldSelectedItem. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DROPDOWN_OLD_SELECTED_ITEM;

/** Fixed property type ShaderTransitionMaterial. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADER_TRANSITION_MATERIAL;

/** Fixed property type ShaderTransitionRenderTargetWidth. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_WIDTH;

/** Fixed property type ShaderTransitionRenderTargetHeight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SHADER_TRANSITION_RENDER_TARGET_HEIGHT;

/** Fixed property type ViewportX. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_X;

/** Fixed property type ViewportY. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_Y;

/** Fixed property type ViewportWidth. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_WIDTH;

/** Fixed property type ViewportHeight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_HEIGHT;

/** Fixed property type ViewportType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_VIEWPORT_TYPE;

/** Fixed property type StartupScene. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_STARTUP_SCENE;

/** Fixed property type WindowMetricsType. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_METRICS_TYPE;

/** Fixed property type WindowX. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_X;

/** Fixed property type WindowY. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_Y;

/** Fixed property type WindowWidth. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_WIDTH;

/** Fixed property type WindowHeight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_HEIGHT;

/** Fixed property type WindowAspectRatio. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_WINDOW_ASPECT_RATIO;

/** Fixed property type MessageArgumentTargetObject. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT;

/** Fixed property type MessageArgumentTargetObjectName. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_MESSAGE_ARGUMENT_TARGET_OBJECT_NAME;

/** Fixed property type TextLabelObject. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TEXT_LABEL_OBJECT;

/** Fixed property type GridLayoutData. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_GRID_LAYOUT_DATA;

/** Fixed property type TargetComponentLocation. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_TARGET_COMPONENT_LOCATION;

/** Fixed property type ButtonPressDuration. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_BUTTON_PRESS_DURATION;

/** Fixed property type DirectionalLight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_DIRECTIONAL_LIGHT;

/** Fixed property type PointLight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_POINT_LIGHT;

/** Fixed property type SpotLight. */
extern const struct KzuPropertyType* KZU_PROPERTY_TYPE_SPOT_LIGHT;


/** Initializes all fixed properties. */
kzsError kzuFixedPropertiesCreate(const struct KzcMemoryManager* memoryManager);

/** Deletes all fixed properties. */
kzsError kzuFixedPropertiesDelete(void);

/** Finds a fixed property type by name. */
struct KzuPropertyType* kzuFixedPropertiesFindPropertyType(kzString name);


#endif
