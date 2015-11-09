#ifndef NDPluginAPSGoogleProto_H
#define NDPluginAPSGoogleProto_H

#include <epicsTypes.h>
#include <asynStandardInterfaces.h>

#include "NDPluginDriver.h"
#include "dataPipe.pb.h"

/** Map parameter enums to strings that will be used to set up EPICS databases
  */
#define NDPluginHexMapTypeString               "HEXMAP_TYPE"           /* (asynInt32,   r/o) Mapoing type: None, HexToSquare, SquareToHex */
#define NDPluginHexMapMaxNewSizeXString        "NEW_SIZE_X"            /* (asynInt32,   r/o) Finish size of image in X dimension */
#define NDPluginHexMapMaxNewSizeYString        "NEW_SIZE_Y"            /* (asynInt32,   r/o) Finish size of image in Y dimension */
#define NDPluginHexMapHexPitchString           "HEX_PITCH"             /* (asynFlt64,   r/o) Pixel Pitch of Detector */


static const char* pluginName = "NDPluginHexMap";

/** Perform transformations (rotations, flips) on NDArrays.   */
class epicsShareClass NDPluginAPSGoogleProto : public NDPluginDriver {
public:
    NDPluginAPSGoogleProto(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int maxBuffers, size_t maxMemory,
                 int priority, int stackSize);
    /* These methods override the virtual methods in the base class */
    void processCallbacks(NDArray *pArray);

protected:
    int NDPluginHexMapType_;
    #define FIRST_HEXMAP_PARAM NDPluginHexMapType_
    int NDPluginHexMapMaxNewSizeX;
    int NDPluginHexMapMaxNewSizeY;
    int NDPluginHexMapPitch;
    #define LAST_HEXMAP_PARAM NDPluginHexMapPitch

private:
    size_t userDims_[ND_ARRAY_MAX_DIMS];
    detectorTest::MyMessage outingMessage;
};
#define NUM_HEXMAP_PARAMS ((int)(&LAST_HEXMAP_PARAM - &FIRST_HEXMAP_PARAM + 1))

#endif
