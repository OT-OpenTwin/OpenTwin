/*
 *	File:		aMSSQLWrapper.h
 *	Package:	akDB
 *
 *  Created on: July 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>

// C++ header
#include <exception>
#include <string>
#include <Windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

namespace ak {

	class UICORE_API_EXPORT aMSSQLWrapper {
	public:

		//! @brief Default constructor
		//! To be able to execute queries a connection must be established (connect())
		aMSSQLWrapper();

		//! @brief Constructor
		//! Will call the connect function
		//! @throw std::exception In case it failed to establish a connection
		aMSSQLWrapper(
			const std::string &		_ipAddress,
			const std::string &		_port,
			const std::string &		_userName,
			const std::string &		_password
		);

		//! @brief Deconstructor
		virtual ~aMSSQLWrapper();

		// ################################################################################################################

		// Connection

		//! @brief Will establish the connection to the SQL server
		//! Will return false if the connection failed.
		//! @param _ipAddress The IP adress of the SQL server
		//! @param _port The port of the SQL server
		//! @param _userName The name of the user
		//! @param _password The user password
		bool connect(
			const std::string &		_ipAddress,
			const std::string &		_port,
			const std::string &		_userName,
			const std::string &		_password
		);

		//! @brief Will establish the connection to the SQL server
		//! Will return false if the connection failed.
		//! @param _ipAddress The IP adress of the SQL server
		//! @param _port The port of the SQL server
		//! @param _userName The name of the user
		//! @param _password The user password
		bool connect(
			const std::wstring &	_ipAddress,
			const std::wstring &	_port,
			const std::wstring &	_userName,
			const std::wstring &	_password
		);

		//! @brief Will establish the connection to the SQL server
		//! Will return false if the connection failed.
		//! @param _connectionString The string containing all the connection information
		bool connect(
			const std::string &		_connectionString
		);

		//! @brief Will establish the connection to the SQL server
		//! Will return false if the connection failed.
		//! @param _connectionString The string containing all the connection information
		bool connect(
			const std::wstring &	_connectionString
		);

		//! @brief Will close the current connection
		void disconnect(void);

		//! @brief Executes SQL query
		//! @return Returns the SQL handle of this query. This handle can be used to retreive the data from the response. The handle must be freed
		//! @throw std::exception If error occurs while allocating the statement handle
		SQLHANDLE executeQuery(const std::wstring & _query);

		// ###################################################################################################################################

		// Getter

		//! @brief Will return true if the connection is established
		bool isConnected(void) const { return m_isConnected; }

		//! @brief Will return true if a table with the specified name does exist
		bool tableExists(const std::wstring & _table);

		// ###################################################################################################################################

		// Helper functions

		//! @brief Will return the last connection error message
		std::wstring getSQLConnectionError(bool _includeState = true);

		//! @brief Will return the last environment error message
		std::wstring getSQLEnvironmentError(bool _includeState = true);

		std::wstring getSQLQueryError(bool _includeState = true);

		//! @brief Will return the last error message for the provided SQLHandle
		std::wstring getSQLError(unsigned int _handleType, const SQLHANDLE & _handle, bool _includeState = true);

		//! @brief Will convert the provided std::string to a std::wstring
		std::wstring convertString(const std::string & _str);

		void freeLastQueryHandle(void);

		static void freeHandle(SQLHANDLE & _handle, unsigned int _handleType);

	protected:

		//! @brief Closes connection to the database and frees associated memory by deallocating the connection and environment SQL handles
		void deallocateHandles();

		bool			m_isConnected;			//! If true, the connection to the DB is established

		SQLHANDLE		m_environmentHandle;	//! Stores environment attributes for SQL Server
		SQLHANDLE		m_connectionHandle;		//! Stores connection attributes to connect to SQL Server
		SQLHANDLE		m_lastQueryHandle;		//! Stores the last query handle

	private:

		aMSSQLWrapper(aMSSQLWrapper&) = delete;
		aMSSQLWrapper& operator = (aMSSQLWrapper&) = delete;
	};

	class UICORE_API_EXPORT aMSSQLConnectionException : public std::exception {
	public:
		aMSSQLConnectionException();
		aMSSQLConnectionException(const char * _errorText);
	};

	class UICORE_API_EXPORT aMSSQLQueryException : public std::exception {
	public:
		aMSSQLQueryException();
		aMSSQLQueryException(const char * _errorText);
	};

}