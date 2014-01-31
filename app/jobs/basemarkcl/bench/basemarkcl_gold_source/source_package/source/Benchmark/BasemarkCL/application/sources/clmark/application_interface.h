/**
 * \file
 * Application interface implementation
 * 
 * Copyright 2011 by Rightware. All rights reserved.
 */
#ifndef APPLICATION_INTERFACE_H
#define APPLICATION_INTERFACE_H


#include <system/debug/kzs_error.h>
#include <system/kzs_types.h>

/** Application states. */
enum ApplicationState
{
    APPLICATION_STATE_NONE, /**< State none, used to signal no state change is necessary. */
    APPLICATION_STATE_MENU, /**< Application state in menu. */
    APPLICATION_STATE_START_TEST_SUITE, /**< Application state starting test suite. */
    APPLICATION_STATE_TEST, /**< Application state running tests. */
    APPLICATION_STATE_QUIT /**< Application wants to quit. */
};


/** Application objects and state variables. */
struct ApplicationData
{
    enum ApplicationState applicationState; /**< Application state. */
    enum ApplicationState applicationStateNext; /**< Application state. */
    struct BenchmarkFramework* framework; /**< Benchmark framework. */
    struct ClMenu* menu; /**< Menu. */
    kzMutableString commandLineParameters; /**< Application command line parameters. */
    kzBool testTerminationRequested; /**< Is the termination of test requested by user. With ESC for example. */
};


/** Adds test to test queue. */
kzsError addTestToQueue(struct BenchmarkFramework* framework, kzUint index, kzBool* out_testExists);
/** Processes autostart tests. */
kzsError clMenuProcessAutoStart(struct ApplicationData* applicationData, struct BenchmarkFramework* framework, struct ClMenu* clMenu, kzBool* out_autoStartActivated);


#endif
