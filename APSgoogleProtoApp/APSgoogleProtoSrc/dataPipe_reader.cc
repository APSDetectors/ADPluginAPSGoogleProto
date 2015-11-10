// helloWorld_reader.cc
// Author: Russell Woods
//   Date: 09/30/15

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "dataPipe.pb.h"
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage:  " << argv[0] << " input message file name" << endl;
		return -1;
	}


	cout << "running helloWorld_reader..." << endl ;

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// Make an instance of a message
	detectorTest::MyMessage readMessage ;

	// Make a new stream from storage
	fstream input(argv[1], ios::in | ios::binary) ;

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
	vector<int> dimVector ;

	if (arrayDataType == "INT8") {
    	// Convert to int8 array
		cout << "Interpreting Data as int8..." << endl ;

		// Parse Dimension String into a Vector
		
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
		charPerDataType = sizeof(signed char)/sizeof(char) ;		// not generic!!!
		cout << "charPerDataType = " << charPerDataType << endl ;

		char Data[valuesData.length()/charPerDataType] ;
		cout << "Data.size = " << valuesData.length()/charPerDataType << endl ;

		strncpy(Data, valuesData.c_str(), valuesData.length()/charPerDataType) ;
//		Data = reinterpret_cast(signed char*)valuesData ;

		for(int i=0; i < dimVector[1]; i++){
			for(int j=0; j < dimVector[0]; j++){
				cout << int(valuesData.c_str()[j+i*dimVector[0]]) << "\t" ;
			}
			cout << endl ;
		}


	}
    else if (arrayDataType == "UINT8") {
    	//convert to uint8 array
		cout << "Interpreting Data as uint8..." << endl ;

		// Parse Dimension String into a Vector
		
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
		charPerDataType = sizeof(unsigned char)/sizeof(char) ;		// not generic!!!
		cout << "charPerDataType = " << charPerDataType << endl ;

		char Data[valuesData.length()/charPerDataType] ;
		cout << "Data.size = " << valuesData.length()/charPerDataType << endl ;

		for(int i=0; i < dimVector[1]; i++){
			for(int j=0; j < dimVector[0]; j++){
				cout << int(valuesData.c_str()[j+i*dimVector[0]]) << "\t" ;
			}
			cout << endl ;
		}



    }
    else if (arrayDataType == "INT16") {
    	//convert to int16 array
    }
    else if (arrayDataType == "UINT16") {
    	//convert to uint16 array
    }
    else if (arrayDataType == "INT32") {
    	//convert to int32 array

		cout << "Interpreting Data as int32..." << endl ;

		// Parse Dimension String into a Vector
		
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
		charPerDataType = sizeof(int)/sizeof(char) ;		// not generic!!!
		cout << "charPerDataType = " << charPerDataType << endl ;

		char Data[valuesData.length()/charPerDataType] ;
		cout << "valuesData.length = " << valuesData.length() << endl ;
		cout << "Data.size = " << valuesData.length()/charPerDataType << endl ;

		strncpy(Data, valuesData.c_str(), valuesData.length()/charPerDataType) ;
//		Data = reinterpret_cast(signed char*)valuesData ;

		int start_char ;
		string tempString ;
		for(int i=0; i < dimVector[1]; i++){
			for(int j=0; j < dimVector[0]; j++){
				start_char = charPerDataType*(j+i*dimVector[0]) ;
				cout << "start_char = " << start_char << "\t" ;

				tempString = valuesData.substr(start_char, 4) ;
				cout << long(tempString.c_str())<< "\t" ;

				
			}
			cout << endl ;
		}
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
