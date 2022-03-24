#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include "EARNStructs.h"

//DELETE ALL OF THIS BEFORE MERGING
//DELETE THIS BEFORE MERGING WITH MAIN BRANCH

#define _CRT_SECURE_NO_WARNINGS

#define STRING_LENGTH 30
const int HeadSize = 84;
enum dataTypeChoice { CreateUserType, LoginType, DepositType, WithdrawType, TransferType, LogoffType };

//using namespace std;
//using namespace EarnStructs;

//bytes 8-12 are the accountID
//bytes 1-16 are together in the beginnning 
//last 2 bytes are after the child object
//Total bytes are 18
//crc is after data
//ie src + head size + data = 132+288 if create account, then crc right after ie starting at 422

class DataTypes {

protected:
	int accountID;

public:
	//virtual void setAccountID(int num) = 0;
	virtual void display() = 0;
};

#define IP_SIZE 10
#define IP_ADDR "127.0.0.1"

class Packet {

	struct Header {

		char toIP[IP_SIZE];
		char fromIP[IP_SIZE];
		//int operationType;
		int objectType;
		int dataSize;
		char TimeOfSend[EarnStructs::VARCHARLEN];
		//int branchID;
		int accountType;				//chequing or savings
		int status;						//0 for default, 1 for success, 2 for failure


	}HEAD;

	char* Data;
	char* TxBuffer;
	int Tail = 0;

public:

	Packet(DataTypes* data, int size, int OpType, int accType);
	Packet(DataTypes* data1, int size1, DataTypes* data2, int size2, int OpType);

	Packet(char* src);									//to populate header

	Packet(int OpType, int AccType);

	void setTime();

	//Would have to create a packet object and use that to serialize everything

	char* serialize(int& size);

	void display();

	int getObjectType();							//get operations for header 

	int getAccountType();

	void setAccountType(int accountType);

	char* getTime();

	int getTail();

	void setStatus(int num);
	int getStatus();


};

//DELETE ALL ABOVE BEFORE MERGING

namespace EarnLogging {

	//Generic Log Object implemented by subtypes
	class EARNLogObject {

		//string used for log comments field since most logs should have it in them...
		std::string logComment;

	public:
		//Constructors

		//Default constructor
		EARNLogObject();

		//Constructor for EARNLogObject, just requite input for this alternative
		EARNLogObject(std::string inputComment);

		//Get the log info about the object as a string to allow it to be written imediately
		virtual std::string getloginfo() = 0;

		//static Columns string for connection log, used in log initalization
		virtual std::string logColumnTemplate() = 0;

		//Get log comment for object
		std::string getLogComment();

		//Set log comment for object
		void setLogComment(std::string inputComment);
	};

	class EARNLogger {
		//path to log file specified during creation
		std::string logFilePath;

		//just cause I needed an input to give both betCurrentTime the same name, and may be useful (currently ALWAYS set to year-month-day since it seems better for timestamps)
		enum DateFormat{YMD_HMS, MDY_HMS, DMY_HMS};

		//Get the current time as a string to write into a .csv, and get current time format
		std::string getCurrentTime(DateFormat logDateFormat);

		//Get log file path for writing log information
		std::string getLogFilePath();

	public:
		EARNLogger(std::string logFilePath);

		bool logData(EARNLogObject* inputObject);
	};

	//enum for types of systems
	enum SystemType{ NOSYSTEM, CLIENTSYSTEM, SERVERSYSTEM};
	
	//Converts enum to string depending on type
	inline const std::string EnumToString(SystemType inputType)
	{
		switch (inputType)
		{
		case NOSYSTEM:		return "NOSYSTEM";
		case CLIENTSYSTEM:	return "CLIENTSYSTEM";
		case SERVERSYSTEM:	return "SERVERSYSTEM";
		default:			return "[Unknown Type]";
		}
	};

	enum RecvOrSend{NODIRECTION, RECEIVE, SEND};
	//Converts enum to string depending on type
	inline const std::string EnumToString(RecvOrSend inputType)
	{
		switch (inputType)
		{
		case NODIRECTION:		return "NODIRECTION";
		case RECEIVE:	return "RECEIVE";
		case SEND:	return "SEND";
		default:			return "[Unknown Type]";
		}
	};

	//Concrete Log Object for connection / packets
	class ConnectionLog :public EARNLogObject {
		Packet* parentPacket;
		SystemType currentSystem;
		RecvOrSend packetDirection;

	public:
		//Constructor

		//Constructor for ConnectionLog Object, requires a packet to be used...
		ConnectionLog(Packet* inputPacket, SystemType inputSystemType, RecvOrSend inputDirection, std::string inputLogComment);

		//Get System Type Enum
		SystemType getSystemType();

		//Get Packet Direction Enum
		RecvOrSend getPacketDirection();

		//Get the log info about the packet as a string to allow it to be written imediately
		std::string getloginfo();

		//static Columns string for connection log, used in log initalization
		std::string logColumnTemplate();

		//change the Packet in the class
		void changePacketInfo(Packet* inputPacket);
	};
}