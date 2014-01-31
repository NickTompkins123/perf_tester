/**
 * \file
 * Property accessors for KzuCameraNode.
 */
#include "kzu_camera.h"
#include "kzu_camera_properties.h"

#include <user/properties/kzu_property_manager.h>
#include <user/properties/kzu_fixed_properties.h>


kzBool kzuCameraNodeGetDisableAspectRatio(const struct KzuCameraNode* cameraNode)
{
    return kzuObjectNodeGetBoolPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_DISABLE_ASPECT_RATIO);
}

kzsError kzuCameraNodeSetDisableAspectRatio(const struct KzuCameraNode* cameraNode, kzBool value)
{
    kzsError result;
    
    result = kzuObjectNodeSetBoolProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_DISABLE_ASPECT_RATIO, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzFloat kzuCameraNodeGetZNear(const struct KzuCameraNode* cameraNode)
{
    return kzuObjectNodeGetFloatPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ZNEAR);
}

kzsError kzuCameraNodeSetZNear(const struct KzuCameraNode* cameraNode, kzFloat value)
{
    kzsError result;
    
    result = kzuObjectNodeSetFloatProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ZNEAR, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzFloat kzuCameraNodeGetZFar(const struct KzuCameraNode* cameraNode)
{
    return kzuObjectNodeGetFloatPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ZFAR);
}

kzsError kzuCameraNodeSetZFar(const struct KzuCameraNode* cameraNode, kzFloat value)
{
    kzsError result;
    
    result = kzuObjectNodeSetFloatProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ZFAR, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

enum KzuProjectionType kzuCameraNodeGetProjectionType(const struct KzuCameraNode* cameraNode)
{
    return /*lint -e{930}*/(enum KzuProjectionType)kzuObjectNodeGetIntPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE);
}

kzsError kzuCameraNodeSetProjectionType(const struct KzuCameraNode* cameraNode, enum KzuProjectionType value)
{
    kzsError result;
    
    result = kzuObjectNodeSetIntProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_PROJECTION_TYPE, /*lint -e{930}*/(kzInt)value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

enum KzuOrthogonalCoordinateSystem kzuCameraNodeGetOrthogonalCoordinateSystemType(const struct KzuCameraNode* cameraNode)
{
    return /*lint -e{930}*/(enum KzuOrthogonalCoordinateSystem)kzuObjectNodeGetIntPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE);
}

kzsError kzuCameraNodeSetOrthogonalCoordinateSystemType(const struct KzuCameraNode* cameraNode, enum KzuOrthogonalCoordinateSystem value)
{
    kzsError result;
    
    result = kzuObjectNodeSetIntProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_COORDINATE_SYSTEM_TYPE, /*lint -e{930}*/(kzInt)value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzFloat kzuCameraNodeGetOrthogonalPlaneHeight(const struct KzuCameraNode* cameraNode)
{
    return kzuObjectNodeGetFloatPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_PLANE_HEIGHT);
}

kzsError kzuCameraNodeSetOrthogonalPlaneHeight(const struct KzuCameraNode* cameraNode, kzFloat value)
{
    kzsError result;
    
    result = kzuObjectNodeSetFloatProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ORTHOGONAL_PLANE_HEIGHT, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

enum KzuFieldOfViewType kzuCameraNodeGetFovType(const struct KzuCameraNode* cameraNode)
{
    return /*lint -e{930}*/(enum KzuFieldOfViewType)kzuObjectNodeGetIntPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_FOV_TYPE);
}

kzsError kzuCameraNodeSetFovType(const struct KzuCameraNode* cameraNode, enum KzuFieldOfViewType value)
{
    kzsError result;
    
    result = kzuObjectNodeSetIntProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_FOV_TYPE, /*lint -e{930}*/(kzInt)value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzFloat kzuCameraNodeGetFov(const struct KzuCameraNode* cameraNode)
{
    return kzuObjectNodeGetFloatPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_FOV);
}

kzsError kzuCameraNodeSetFov(const struct KzuCameraNode* cameraNode, kzFloat value)
{
    kzsError result;
    
    result = kzuObjectNodeSetFloatProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_FOV, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}

kzFloat kzuCameraNodeGetOrientationAngle(const struct KzuCameraNode* cameraNode)
{
    return kzuObjectNodeGetFloatPropertyDefault(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ORIENTATION_ANGLE);
}

kzsError kzuCameraNodeSetOrientationAngle(const struct KzuCameraNode* cameraNode, kzFloat value)
{
    kzsError result;
    
    result = kzuObjectNodeSetFloatProperty(kzuCameraNodeToObjectNode(cameraNode), KZU_PROPERTY_TYPE_CAMERA_ORIENTATION_ANGLE, value);
    kzsErrorForward(result);
    
    kzsSuccess();
}
