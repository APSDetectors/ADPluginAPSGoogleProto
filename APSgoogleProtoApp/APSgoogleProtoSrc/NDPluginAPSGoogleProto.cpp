/*
 * NDPluginAPSGoogleProto.cpp
 *
 * APSGoogleProto plugin
 *
 * Take an NDArray an outputs it to a file/pipe/etc, using a Google Protocol Buffer.
 *
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
#include <epicsString.h>
#include <epicsMutex.h>
#include <iocsh.h>
#include <asynDriver.h>
#include <epicsExport.h>
#include "NDPluginAPSGoogleProto.h"
#include <string>
#include "NDPluginFile.h"
#include <iostream>


//Copied in from NDFileNull.cpp
asynStatus NDPluginAPSGoogleProto::openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray)
{
  this->output.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
  
  if (this->output.is_open())
    return(asynSuccess);
  else
    return asynError;
}

/** Writes single NDArray to the Null file.
  * \param[in] pArray Pointer to the NDArray to be written
  */
asynStatus NDPluginAPSGoogleProto::writeFile(NDArray *pArray)
{
  NDAttributeList *pScratchNDAttrList = new NDAttributeList;
  NDArrayInfo arrayInfo;

  static const char* functionName = "writeFile";
  
  this->getAttributes(pScratchNDAttrList);
  pArray->pAttributeList->copy(pScratchNDAttrList);
  
  
  pArray->getInfo(&arrayInfo);
  
  outgoingMessage.set_valuesdata(pArray->pData,arrayInfo.totalBytes);
  outgoingMessage.set_numdimdata(pArray->ndims);
  outgoingMessage.set_unquieid(pArray->uniqueId);
  outgoingMessage.set_timestamp(pArray->timeStamp);
  // creates array dimension string from NDArray info
  std::string dimStr="";
  char tempStr[50];
  int i, size;
  for (i=0;i<pArray->ndims; i++){
    size=sprintf(tempStr,"%d,",pArray->dims[i].size);
    dimStr.append(tempStr, size);  
  }
  outgoingMessage.set_dimdata(dimStr);
  
  //Sets NDArray Datatype info 
  switch (pArray->dataType) {
      case NDInt8:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_INT8);
        break;
      case NDUInt8:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_UINT8);
        break;
      case NDInt16:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_INT16);
        break;
      case NDUInt16:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_UINT16);
        break;
      case NDInt32:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_INT32);
        break;
      case NDUInt32:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_UINT32);
        break;
      case NDFloat32:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_FLOAT32);
        break;
      case NDFloat64:
        outgoingMessage.set_datatype(APSdet_GPB::AGP_ArrayDataType_FLOAT64);
        break;
    }
    
  //NDAttribute loading
  NDAttribute *CurrentAttr; 
  NDAttrDataType_t CurrentAttrDataType;
  int AttributeCnt = 0;
  std::string AttrNames="",AttrTypes="", AttrValues=""; 
  NDAttrValue attrValue;
  size_t attrDataSize;
  char charValue[(sizeof(epicsFloat64)/sizeof(char))];
  
  CurrentAttr = pScratchNDAttrList->next(NULL); //Get First Attribute

  while (CurrentAttr !=NULL){
    AttributeCnt++;
    size=sprintf(tempStr,"%s,",CurrentAttr->getName());
    AttrNames.append(tempStr,size);

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

  }

  outgoingMessage.set_nameattrs(AttrNames);
  outgoingMessage.set_numattrs(AttributeCnt);
  outgoingMessage.set_typeattrs(AttrTypes);
  outgoingMessage.set_valuesattrs(AttrValues);
  
  
  
  outgoingMessage.SerializeToOstream(&output) ;
    
  return(asynSuccess);
}

/** Reads single NDArray from a Null file; NOT CURRENTLY IMPLEMENTED.
  * \param[in] pArray Pointer to the NDArray to be read
  */
asynStatus NDPluginAPSGoogleProto::readFile(NDArray **pArray)
{
    return asynError;
}


/** Closes the Null file. */
asynStatus NDPluginAPSGoogleProto::closeFile()
{
  if (this->output.is_open())
    this->output.close();
    
  return asynSuccess;
}







/** Constructor for NDPluginAPSGoogleProto; most parameters are simply passed to NDPluginDriver::NDPluginDriver.
  * After calling the base class constructor this method sets reasonable default values for all of the
  * APS Google Protocol Bruffer parameters.
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
  : NDPluginFile(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, NUM_APS_GPB_PARAMS, maxBuffers, maxMemory,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize)
   /*                
  : NDPluginFile(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, NUM_APS_GPB_PARAMS,
                   2, 0, asynGenericPointerMask, asynGenericPointerMask, 
                   ASYN_CANBLOCK, 1, priority, stackSize)
                   */
{
  static const char *functionName = "NDPluginAPSGoogleProto";

  for (int i = 0; i < ND_ARRAY_MAX_DIMS; i++) {
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
