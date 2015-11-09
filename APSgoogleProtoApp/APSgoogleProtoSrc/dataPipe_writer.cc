// helloWorld_sender.cc
// Author: Russell Woods
//   Date: 09/30/15

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "dataPipe.pb.h"
using namespace std;

int main() {
	cout << "running dataPipe_sender..." << endl ;

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// Make a new object (instance of type MyMessage)
	detectorTest::MyMessage testMessage ;
	
	// Set the field values of message

	// Data 
	testMessage.set_numdimdata(2) ;
	testMessage.set_dimdata("10, 10") ;

	//-----------------------------------------
	// Read image into a single string of bytes
	string line ;
	string totalString ;
	ifstream dataInput("simple_spectra.dat", ios::in ) ;

	while (dataInput.good()) 
	{
		// Get whole line
		getline(dataInput, line) ;
		
		// Store in a vector (might be useful in the future)
		istringstream instring(line) ;
		int n ;
		vector<int> v ;

		while(instring >> n)
		{
			cout << n << endl ;
			v.push_back(n) ;
		}

		// Append it to totalString
		//totalString += tempString ;
	}

	//cout << "totalString = " << totalString << endl ;

	// Set
	//testMessage.set_valuesdata(totalString) ;
	
	//----------------------------------------------------------------------
	// Make a stream object
	fstream output("tempStorage.dat", ios::out | ios::trunc | ios::binary) ;
	
	// stream the message to file
	testMessage.SerializeToOstream(&output) ;
}
