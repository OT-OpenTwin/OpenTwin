// @otlicense
// File: StartArgumentParser.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end



// OpenTwin header
#include <OTCore/Serializable.h>

// Qt header
#include <QtCore/qstring.h>

#pragma once
namespace ot {
	class StartArgumentParser
	{
		OT_DECL_DEFMOVE(StartArgumentParser)
		OT_DECL_DEFCOPY(StartArgumentParser)
	public:
		StartArgumentParser() = default;
		~StartArgumentParser() = default;

		bool parse();

		void setDataPath(const QString& _dataPath) { m_dataPath = _dataPath; }
		QString getDataPath() const { return m_dataPath; }

		void setCollectionName(const QString& _collectionName) { m_collectionName = _collectionName; }
		QString getCollectionName() const { return m_collectionName; }

		void setLmsUrl(const QString& _lmsUrl) { m_lmsUrl = _lmsUrl; }
		QString getLmsUrl() const { return m_lmsUrl; }

		void setDatabasePsw(const QString& _databasePsw) { m_databasePsw = _databasePsw; }
		QString getDatabasePsw() const { return m_databasePsw; }


	private:
		QString m_dataPath;
		QString m_collectionName;
		QString m_lmsUrl;
		QString m_databasePsw;
	};
}
