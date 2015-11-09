/*
 * NDPluginAPSGoogleProto.cpp
 *
 * HexMap plugin
 * Author: Matthew Moore
 *         Russell Woods
 * Created Nov. 5, 2015
 *
 * Change Log:
 *
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <epicsString.h>
#include <epicsMutex.h>
#include <iocsh.h>

#include <asynDriver.h>

#include <epicsExport.h>
#include "NDPluginDriver.h"
#include "NDPluginAPSGoogleProto.h"
#include "dataPipe.pb.h"
#include <string>
#include <fstream>
#include <iostream>




/** Callback function that is called by the NDArray driver with new NDArray data.
  * Grabs the current NDArray and applies the selected transforms to the data.  Apply the transforms in order.
  * \param[in] pArray  The NDArray from the callback.
  */
void NDPluginAPSGoogleProto::processCallbacks(NDArray *pArray){
  NDArray *pScratch=NULL;
  NDAttributeList *pScratchNDAttrList = new NDAttributeList;
  NDArrayInfo arrayInfo;

  unsigned int numRows, rowSize;

  static const char* functionName = "processCallbacks";


  /* Call the base class method */
  NDPluginDriver::processCallbacks(pArray);

  /* Do the computationally expensive code with the lock released */
  this->unlock();
  
  /* Copy Array */
  
  pScratch= this->pNDArrayPool->copy( pArray, NULL, 1);
  
  this->getAttributes(pScratchNDAttrList);
  pArray->pAttributeList->copy(pScratchNDAttrList);
  
  this->lock();
  
  
  pScratch->getInfo(&arrayInfo);
  //creates byte array from NDArray data
  char charArray[(arrayInfo.totalBytes/sizeof(char))];
  memcpy(&charArray, reinterpret_cast<char*>(pScratch->pData),arrayInfo.totalBytes);
  
  outingMessage.set_valuesdata(charArray);
  outingMessage.set_numdimdata(pScratch->ndims);
  
  // creates array dimension string from NDArray info
  std::string dimStr="";
  char tempStr[50];
  int i, size;
  for (i=0;i<pScratch->ndims; i++){
    size=sprintf(tempStr,"%d,",pScratch->dims[i].size);
    dimStr.append(tempStr, size);  
  }
  outingMessage.set_dimdata(dimStr);
  
  //Sets NDArray Datatype info 
  switch (pScratch->dataType) {
      case NDInt8:
        outingMessage.set_datatype("INT8");
        break;
      case NDUInt8:
        outingMessage.set_datatype("UINT8");
        break;
      case NDInt16:
        outingMessage.set_datatype("INT16");
        break;
      case NDUInt16:
        outingMessage.set_datatype("UINT16");
        break;
      case NDInt32:
        outingMessage.set_datatype("INT32");
        break;
      case NDUInt32:
        outingMessage.set_datatype("UINT32");
        break;
      case NDFloat32:
        outingMessage.set_datatype("FLOAT32");
        break;
      case NDFloat64:
        outingMessage.set_datatype("FLOAT64");
        break;
    }
    
  //NDAttribute loading
  NDAttribute *CurrentAttr; 
  //NDAttribute *pAttribute;
  NDAttrDataType_t CurrentAttrDataType;
  int AttributeCnt = 0;
  std::string AttrNames="",AttrTypes="", AttrValues=""; 
  NDAttrValue attrValue;
  size_t attrDataSize;
  char charValue[(sizeof(epicsFloat64)/sizeof(char))];
  
  CurrentAttr = pScratchNDAttrList->next(NULL); //Get First Attribute
  printf("There are %d attributes.\n",pScratchNDAttrList->count());
  printf("Gets first Attr...\n");
  while (CurrentAttr !=NULL){
    AttributeCnt++;
    size=sprintf(tempStr,"%s,",CurrentAttr->getName());
    AttrNames.append(tempStr,size);
    //std::cout << AttrNames << endl;
    CurrentAttr->getValueInfo(&CurrentAttrDataType, &attrDataSize);
    
    
    CurrentAttr->getValue(CurrentAttrDataType, &attrValue, attrDataSize);
    if (CurrentAttrDataType==NDAttrString){
        AttrTypes+=  "STRING,";
        AttrValues.append((char*)&attrValue);
    }
    else {
      switch (CurrentAttrDataType) {
        case NDAttrInt8:
          AttrTypes+=  "INT8,";
          *charValue=attrValue.i8;
          break;
        case NDAttrUInt8:
          AttrTypes+=  "UINT8,";
          *charValue=attrValue.ui8;
          break;
        case NDAttrInt16:
          AttrTypes+=  "INT16,";
          *charValue=attrValue.i16;
          break;
        case NDAttrUInt16:
          AttrTypes+=  "UINT16,";
          *charValue=attrValue.ui16;
          break;
        case NDAttrInt32:
          AttrTypes+=  "INT32,";
          *charValue=attrValue.i32;
          break;
        case NDAttrUInt32:
          AttrTypes+=  "UINT32,";
          *charValue=attrValue.ui32;
          break;
        case NDAttrFloat32:
          AttrTypes+=  "FLOAT32,";
          *charValue=attrValue.f32;
          break;
        case NDAttrFloat64:
          AttrTypes+=  "FLOAT64,";
          *charValue=attrValue.f64;
          break;
        
        case NDAttrUndefined:
          AttrTypes+=  "UNKNOWN,";
          break;
      }
      AttrValues.append(charValue);
    }
    
    CurrentAttr = pScratchNDAttrList->next(CurrentAttr); //Get Next Attribute
    printf("Gets another Attr...\n");
  }
  printf("Done with Attr...\n");
  outingMessage.set_nameattrs(AttrNames);
  outingMessage.set_numattrs(AttributeCnt);
  outingMessage.set_typeattrs(AttrTypes);
  
  std::fstream output("SIM_outdata.dat", std::ios::out | std::ios::trunc | std::ios::binary) ;
  outingMessage.SerializeToOstream(&output) ;
  
  int arrayCallbacks = 0;
  getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
  if (arrayCallbacks == 1) {
    //getAttributes(pTransformed->pAttributeList);
    this->unlock();
    //printf("Doing Callback\n");
    //doCallbacksGenericPointer(pTransformed, NDArrayData, 0);
    this->lock();
  }

  callParamCallbacks();
}




/** Constructor for NDPluginHexMap; most parameters are simply passed to NDPluginDriver::NDPluginDriver.
  * After calling the base class constructor this method sets reasonable default values for all of the
  * HexMap parameters.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this plugin can hold when
  *      NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease the number of dropped arrays,
  *      at the expense of more NDArray buffers being allocated from the underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the NDPluginDriverBlockingCallbacks flag.
  *      0=callbacks are queued and executed by the callback thread; 1 callbacks execute in the thread
  *      of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of NDArray callbacks.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *      allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *      allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
NDPluginAPSGoogleProto::NDPluginAPSGoogleProto(const char *portName, int queueSize, int blockingCallbacks,
             const char *NDArrayPort, int NDArrayAddr, int maxBuffers, size_t maxMemory,
             int priority, int stackSize)
  /* Invoke the base class constructor */
  : NDPluginDriver(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, NUM_HEXMAP_PARAMS, maxBuffers, maxMemory,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   ASYN_MULTIDEVICE, 1, priority, stackSize)
{
  static const char *functionName = "NDPluginAPSGoogleProto";
  int i;
  
  createParam(NDPluginHexMapTypeString,          asynParamInt32,   &NDPluginHexMapType_);
  //createParam(NDPluginHexMapMaxSizeXString,      asynParamInt32,   &NDPluginHexMapMaxSizeX);
  //createParam(NDPluginHexMapMaxSizeYString,      asynParamInt32,   &NDPluginHexMapMaxSizeY);
  createParam(NDPluginHexMapMaxNewSizeXString,   asynParamInt32,   &NDPluginHexMapMaxNewSizeX);
  createParam(NDPluginHexMapMaxNewSizeYString,   asynParamInt32,   &NDPluginHexMapMaxNewSizeY);
  createParam(NDPluginHexMapHexPitchString,      asynParamFloat64, &NDPluginHexMapPitch);
  
  for (i = 0; i < ND_ARRAY_MAX_DIMS; i++) {
    this->userDims_[i] = i;
  }
  
  /* Set the plugin type string */
  setStringParam(NDPluginDriverPluginType, "NDPluginAPSGoogleProto");
  

  // Enable ArrayCallbacks.  
  // This plugin currently ignores this setting and always does callbacks, so make the setting reflect the behavior
  setIntegerParam(NDArrayCallbacks, 1);

  /* Try to connect to the array port */
  connectToArrayPort();
}

/** Configuration command */
extern "C" int NDAPSGoogleProtoConfigure(const char *portName, int queueSize, int blockingCallbacks,
                                    const char *NDArrayPort, int NDArrayAddr,
                                    int maxBuffers, size_t maxMemory,
                                    int priority, int stackSize)
{
  new NDPluginAPSGoogleProto(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
              maxBuffers, maxMemory, priority, stackSize);
  return(asynSuccess);
}

/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg1 = { "frame queue size",iocshArgInt};
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt};
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString};
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt};
static const iocshArg initArg5 = { "maxBuffers",iocshArgInt};
static const iocshArg initArg6 = { "maxMemory",iocshArgInt};
static const iocshArg initArg7 = { "priority",iocshArgInt};
static const iocshArg initArg8 = { "stackSize",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
                                            &initArg1,
                                            &initArg2,
                                            &initArg3,
                                            &initArg4,
                                            &initArg5,
                                            &initArg6,
                                            &initArg7,
                                            &initArg8};
static const iocshFuncDef initFuncDef = {"NDAPSGoogleProtoConfigure",9,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
  NDAPSGoogleProtoConfigure(args[0].sval, args[1].ival, args[2].ival,
                       args[3].sval, args[4].ival, args[5].ival,
                       args[6].ival, args[7].ival, args[8].ival);
}

extern "C" void NDAPSGoogleProtoRegister(void)
{
  iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
  epicsExportRegistrar(NDAPSGoogleProtoRegister);
}
