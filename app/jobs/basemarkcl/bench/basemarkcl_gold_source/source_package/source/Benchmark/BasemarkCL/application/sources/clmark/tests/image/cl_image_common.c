/**
* \file
* OpenCL benchmark image and video test common utilities.
* 
* Copyright 2011 by Rightware. All rights reserved.
*/
#include "cl_image_common.h"

#include <benchmarkutil/bf_benchmark_framework.h>

#include <user/material/kzu_material.h>
#include <user/project/kzu_project_loader_material.h>
#include <user/project/kzu_project_loader_mesh.h>
#include <user/scene_graph/kzu_mesh.h>


kzsError clImageCommonAssignTexture(struct BenchmarkFramework* framework, enum ImageTestType type)
{
    kzsError result;
    struct KzuMesh* mesh;
    struct KzuMaterial* material;
    kzString path;

    switch(type)
    {
        case IMAGE_TEST_TYPE_COLOR_CORRECTION:
        {
            path = "Materials/image_test_textured/image_test_color_c";
            break;
        }
        case IMAGE_TEST_TYPE_NOISE_REDUCTION:
        {
            path = "Materials/image_test_textured/image_test_noise_reduction";
            break;
        }
        case IMAGE_TEST_TYPE_SHARPENING:
        {
            path = "Materials/image_test_textured/image_test_sharpening";
            break;
        }
        case IMAGE_TEST_TYPE_SMOOTHING:
        {
            path = "Materials/image_test_textured/image_test_smooth";
            break;
        }
        case IMAGE_TEST_TYPE_SURFACE_SMOOTHING:
        default:
        {
            path = "Materials/image_test_textured/image_test_surface_smooth";
            break;
        }
    }

    result = kzuProjectLoaderLoadMaterial(bfGetProject(framework), path, &material);
    kzsErrorForward(result);
    result = kzuProjectLoaderLoadMesh(bfGetProject(framework), "Scenes/image_test_scene/Root Node/Foreground/image_test_TestDescriptionPlane/Plane Mesh", &mesh);
    kzsErrorForward(result);
    kzuMeshSetMaterial(mesh, material);

    kzsSuccess();
}

