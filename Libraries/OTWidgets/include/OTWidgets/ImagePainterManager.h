//! @file ImagePainterManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <string>
#include <qvariant.h>

namespace ot {

	class ImagePainter;

	class ImagePainterManager {
	public:
		static ImagePainterManager& instance(void);

		void paintImage(const std::string& _key, QPainter* _painter, const QRectF& _bounds) const;

		ImagePainter* getPainter(const std::string& _key) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data manamgement

		//! \brief Stores the painter under the given name.
		//! If an entry for the given key already exists the painter won't be added.
		//! \note The ImagePainterManager takes ownership of the painter.
		void addImagePainter(const std::string& _key, ImagePainter* _painter);

		//! \brief Creates a ImagePainter from the imported file.
		//! If an entry for the file path already exists the operation will be skipped.
		void importFromFile(const std::string& _fileSubPath);

		//! \brief Returns true if an image painter for the given key exists.
		bool contains(const std::string& _key) const { return m_painter.find(_key) != m_painter.end(); };

		//! \brief Destroys all painter and clears the map.
		void clear(void);

		// ###########################################################################################################################################################################################################################################################################################################################
	
		// Private

	private:
		std::map<std::string, ImagePainter*> m_painter;

		ImagePainter* importPng(const std::string& _subPath);
		ImagePainter* importSvg(const std::string& _subPath);

		ImagePainterManager();
		~ImagePainterManager();
	};

}
