#pragma once

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace OToolkitAPI {

	class ProjectCollection : public QObject {
		Q_OBJECT
	public:
		static ProjectCollection& instance(void);

		// ####################################################################################################################

		//! @brief Will add 
		void addImportDirectory(const QString& _directory);

		void startImport(void);

		// ####################################################################################################################

	signals:
		void importFinished(void);
		void importFailed(const QString& _errorMessage);

	private:
		void importWorker(void);

		ProjectCollection();
		virtual ~ProjectCollection();

		QStringList		m_importDirectories;

	};

}