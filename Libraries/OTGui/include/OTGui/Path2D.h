// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>

#pragma warning(disable:4251)

namespace ot {

	//! @class Path2D
	//! @brief The Path2D class describes a path consisting of different types of lines.
	class OT_GUI_API_EXPORT Path2D : public Serializable {
	public:
		//! @brief The type of a path entry.
		enum PathEntryType {
			LineType,
			BerzierType
		};

		//! @brief A path entry contains the
		struct PathEntry {
			PathEntryType type; //! @brief Entry type.
			Point2D start; //! @brief Start position.
			Point2D stop; ///! @brief Stop position.
			Point2D control1; //! @brief Control point 1.
			Point2D control2; //! @brief Control point 2.
		};

		//! @brief Default constructor.
		Path2D();

		//! @brief Assignment constructor.
		//! @param _startPos The initial position of the path (same as calling moveTo())
		Path2D(const Point2D& _startPos);

		//! @brief Copy constructor.
		Path2D(const Path2D& _other);

		//! @brief Destructor.
		virtual ~Path2D();

		//! @brief Assignment operator.
		Path2D& operator = (const Path2D& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Returns true if the path contains at least one entry, the entries are interconnected (end of a entry is start of another) and the start point is also the final end point.
		bool isClosed(void);

		//! @brief Moves the current position.
		//! No line will be added.
		//! @param _pos The position to move to (absolute).
		void moveTo(const Point2D& _pos) { m_pos = _pos; };

		//! @brief Moves the current position by the delta.
		//! @param _d Move distance (incremental).
		void moveBy(const Point2D& _d) { m_pos += _d; };

		//! @brief Adds a line from the current position to the provided destination.
		//! The current position will be set to the destination.
		//! @param _dest The line destination.
		void lineTo(const Point2D& _dest);

		//! @brief Adds a berzier curve from the current position to the provided destination by taking the two control points into account.
		//! The current position will be set to the destination.
		//! @param _ct1 Control point 1.
		//! @param _ct2 Control point 2.
		//! @param _dest The curve destination.
		void berzierTo(const Point2D& _ct1, const Point2D& _ct2, const Point2D& _dest);

		//! @brief Remove all the entries from the path.
		//! @note Note that the current position wont be changed.
		void clear(void);

		//! @brief Returns the current entries.
		const std::list<PathEntry>& getEntries(void) const { return m_entries; };

	private:
		Point2D m_pos; //! @brief Current position.
		std::list<PathEntry> m_entries; //! @brief Path entries.

		//! @brief Returns a string representation of the provided path entry type.
		static std::string toString(PathEntryType _type);

		//! @brief Returns the path entry type represented by the provided string.
		static PathEntryType toPathEntryType(const std::string& _type);
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class Path2DF
	//! @brief The Path2DF class describes a path consisting of different types of lines.
	class OT_GUI_API_EXPORT Path2DF : public Serializable {
	public:
		//! @brief The type of a path entry.
		enum PathEntryType {
			LineType,
			BerzierType
		};

		//! @brief A path entry contains the
		struct PathEntry {
			PathEntryType type; //! @brief Entry type.
			Point2DD start; //! @brief Start position.
			Point2DD stop; ///! @brief Stop position.
			Point2DD control1; //! @brief Control point 1.
			Point2DD control2; //! @brief Control point 2.
		};

		//! @brief Default constructor.
		Path2DF();

		//! @brief Assignment constructor.
		//! @param _startPos The initial position of the path (same as calling moveTo())
		Path2DF(const Point2DD& _startPos);

		//! @brief Copy constructor.
		Path2DF(const Path2DF& _other);

		//! @brief Destructor.
		virtual ~Path2DF();

		//! @brief Assignment operator.
		Path2DF& operator = (const Path2DF& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Returns true if the path contains at least one entry, the entries are interconnected (end of a entry is start of another) and the start point is also the final end point.
		bool isClosed(void);

		//! @brief Moves the current position.
		//! No line will be added.
		//! @param _pos The position to move to (absolute).
		void moveTo(const Point2DD& _pos) { m_pos = _pos; };

		//! @brief Moves the current position by the delta.
		//! @param _d Move distance (incremental).
		void moveBy(const Point2DD& _d) { m_pos += _d; };

		//! @brief Adds a line from the current position to the provided destination.
		//! The current position will be set to the destination.
		//! @param _dest The line destination.
		void lineTo(const Point2DD& _dest);

		//! @brief Adds a berzier curve from the current position to the provided destination by taking the two control points into account.
		//! The current position will be set to the destination.
		//! @param _ct1 Control point 1.
		//! @param _ct2 Control point 2.
		//! @param _dest The curve destination.
		void berzierTo(const Point2DD& _ct1, const Point2DD& _ct2, const Point2DD& _dest);

		//! @brief Remove all the entries from the path.
		//! @note Note that the current position wont be changed.
		void clear(void);

		//! @brief Returns the current entries.
		const std::list<PathEntry>& getEntries(void) const { return m_entries; };

	private:
		Point2DD m_pos; //! @brief Current position.
		std::list<PathEntry> m_entries; //! @brief Path entries.

		//! @brief Returns a string representation of the provided path entry type.
		static std::string toString(PathEntryType _type);

		//! @brief Returns the path entry type represented by the provided string.
		static PathEntryType toPathEntryType(const std::string& _type);
	};

}