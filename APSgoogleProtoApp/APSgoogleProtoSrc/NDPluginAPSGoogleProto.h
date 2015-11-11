#ifndef NDPluginAPSGoogleProto_H
#define NDPluginAPSGoogleProto_H

#include <epicsTypes.h>
#include <asynStandardInterfaces.h>
#include <fstream>
#include "NDPluginFile.h"
#include "APSdet_GPB.pb.h"


static const char* pluginName = "NDPluginAPSGoogleProto";

/** Perform transformations (rotations, flips) on NDArrays.   */
class epicsShareClass NDPluginAPSGoogleProto : public NDPluginFile {
public:
    NDPluginAPSGoogleProto(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int maxBuffers, size_t maxMemory,
                 int priority, int stackSize);
    /* These methods override the virtual methods in the base class */
    //void processCallbacks(NDArray *pArray);
    virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
    virtual asynStatus readFile(NDArray **pArray);
    virtual asynStatus writeFile(NDArray *pArray);
    virtual asynStatus closeFile();

protected:
    //None

private:
    size_t userDims_[ND_ARRAY_MAX_DIMS];
    APSdet_GPB::AGP outgoingMessage;
    std::fstream output;
};
#define NUM_APS_GPB_PARAMS 0

#endif
