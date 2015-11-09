// helloWorld_reader.cc
// Author: Russell Woods
//   Date: 09/30/15

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "dataPipe.pb.h"
using namespace std;

int main() {
	cout << "running helloWorld_reader..." << endl ;

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// Make an instance of a message
	detectorTest::MyMessage readMessage ;

	// Make a new stream from storage
	fstream input("SIM_outdata.dat", ios::in | ios::binary) ;

	// Read and parse storage into message
	readMessage.ParseFromIstream(&input) ;
	
	int nDims = readMessage.numdimdata() ;
	string dimStr = readMessage.dimdata() ;
	string arrayDataType = readMessage.datatype() ;
	string valuesData = readMessage.valuesdata() ;
	int numAttrs = readMessage.numattrs() ;
	string nameAttrs = readMessage.nameattrs() ;

  
	cout << "nDims = " << nDims << endl ;
	cout << "dimStr = " << dimStr << endl ;
	cout << "arrayDataType = " << arrayDataType << endl ;
	cout << "numAttrs = " << numAttrs << endl ;
	cout << "nameAttrs = " << nameAttrs << endl ;
	cout << "valuesData = " << valuesData << endl ;

	int charPerDataType;
	if (arrayDataType == "INT8") {
    	// Convert to int8 array
		cout << "Interpreting Data as int8..." << endl ;

		// Parse Dimension String into a Vector
		vector<int> dimVector ;
		stringstream ss(dimStr) ;
		int dimInt ;
		while(ss >> dimInt) {
			dimVector.push_back(dimInt) ;
			if(ss.peek()==',' || ss.peek()==' ') {
				ss.ignore() ;
			}
		}

		//Check contents
		cout << "dimVector[0] = " << dimVector[0] << endl ;
		cout << "dimVector[1] = " << dimVector[1] << endl ;

		// Parse Data into a Vector
		
		// Something like this.... MDM
		charPerDataType = sizeof(signed char)/sizeof(char) ;
		cout << "charPerDataType = " << charPerDataType << endl ;

		signed char Data[valuesData.length()/charPerDataType] ;
		Data = <signed char>valuesData ;

	}
    else if (arrayDataType == "UINT8") {
    	//convert to uint8 array
    }
    else if (arrayDataType == "INT16") {
    	//convert to int16 array
    }
    else if (arrayDataType == "UINT16") {
    	//convert to uint16 array
    }
    else if (arrayDataType == "INT32") {
    	//convert to int32 array
    }
    else if (arrayDataType == "UINT32") {
    	//convert to uint32 array
    }
    else if (arrayDataType == "FLOAT32") {
    	//convert to float32 array
    }
    else if (arrayDataType == "FLOAT64") {
    	//convert to float64 array
    }

//End
}
