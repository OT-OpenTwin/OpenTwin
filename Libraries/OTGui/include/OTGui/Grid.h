//! @file Grid.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Color.h"
#include "OTCore/Point2D.h"
#include "OTCore/Serializable.h"
#include "OTGui/PenCfg.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class Painter2D;

	//! \brief The Grid describes how a 2D Grid should look and behave like.
	class OT_GUI_API_EXPORT Grid : public Serializable {
	public:
		//! \enum GridFlag
		//! \brief A GridFlag describe the grid appearance.
		enum GridFlag {
			NoGridFlags = 0x0000, //! \brief No grid flags.
			ShowNormalLines = 0x0001, //! \brief Basic lines should be drawn.
			ShowWideLines = 0x0002, //! \brief Wide lines should be drawn.
			ShowCenterCross = 0x0004, //! \brief Center cross should be drawn.
			NoGridLineMask = 0xFFF0, //! \brief Mask used to check if no grid line flags are set.

			//! \brief Auto scaling is enabled.
			AutoScaleGrid = 0x0010
		};
		//! \typedef GridFlags
		//! \brief GridFlags are use describe the grid appearance.
		typedef Flags<GridFlag> GridFlags;

		//! \brief The GridSnapMode describe if and how the grid snapping should be performed.
		enum GridSnapMode {
			NoGridSnap,     //! \brief No grid snapping.
			SnapTopLeft, //! \brief Items will snap to the grid. The top left corner of an item will be used as source for snapping.
			SnapCenter //! \brief Items will snap to the grid. The center point of an item will be used as source for snapping.
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Static methods

		static std::string toString(GridFlag _flag);
		static GridFlag stringToGridFlag(const std::string& _flag);
		static std::list<std::string> toStringList(const GridFlags& _flags);
		static GridFlags stringListToGridFlags(const std::list<std::string>& _flags);

		static std::string toString(GridSnapMode _snapMode);
		static GridSnapMode stringToGridSnapMode(const std::string& _snapMode);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		Grid();
		Grid(double _defaultGridLineWidth, const GridFlags& _flags, GridSnapMode _snapMode);
		Grid(const Point2D& _gridStep, const Point2D& _gridWideEvery, const PenFCfg& _gridLineStyle, const GridFlags& _flags, GridSnapMode _snapMode);
		Grid(const Grid& _other);
		virtual ~Grid();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		Grid& operator = (const Grid& _other);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Helper

		//! \brief Snaps the provided point to the grid if snapping is enabled.
		Point2D snapToGrid(const Point2D& _pt) const;

		//! \brief Snaps the provided point to the grid if snapping is enabled.
		Point2DF snapToGrid(const Point2DF& _pt) const;

		//! \brief Snaps the provided point to the grid if snapping is enabled.
		Point2DD snapToGrid(const Point2DD& _pt) const;

		//! \brief Snaps the provided point to the specified grid.
		static Point2D snapToGrid(const Point2D& _pt, const Point2D& _gridStep);

		//! \brief Snaps the provided point to the specified grid.
		static Point2DF snapToGrid(const Point2DF& _pt, const Point2D& _gridStep);

		//! \brief Snaps the provided point to the specified grid.
		static Point2DD snapToGrid(const Point2DD& _pt, const Point2D& _gridStep);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! \brief Sets/Unsets the provided flag.
		//! \param _flag Flag to set/unset.
		//! \param _active If true the flag will be set, otherwise unset.
		//! \see GridFlags
		void setGridFlag(GridFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		
		//! \brief Sets the current GridFlags.
		//! \see GridFlags
		void setGridFlags(const GridFlags& _flags) { m_flags = _flags; };

		//! \brief Returns the current GridFlags.
		//! \see GridFlags
		const GridFlags& getGridFlags(void) const { return m_flags; };

		//! \brief Sets the current grid step size.
		//! \param _step The step distance that will be set for X and Y.
		void setGridStep(int _step) { m_gridStep = Point2D(_step, _step); };

		//! \brief Sets the current grid step size.
		//! \see getGridStep
		void setGridStep(const Point2D& _step) { m_gridStep = _step; };

		//! \brief Returns the current grid step size.
		//! The current step is the distance between every grid line on the X and Y axis.
		const Point2D& getGridStep(void) const { return m_gridStep; };

		//! \brief Set the wide grid line counter.
		//! \param _stepCount The step count for the X and Y axis.
		//! \see getWideGridLineCounter
		void setWideGridLineCounter(int _stepCount) { m_gridWideEvery = Point2D(_stepCount, _stepCount); };

		//! \brief Set the wide grid line counter.
		//! \see getWideGridLineCounter
		void setWideGridLineCounter(const Point2D& _stepCount) { m_gridWideEvery = _stepCount; };

		//! \brief Returns the counter limit for wide grid lines.
		//! The counter must be greater than 1 to be valid.
		//! For a counter of (2, 5) every 2nd vertical (-x to +x) and every 5th horizontal (-y to +y) grid line will be wide.
		const Point2D& getWideGridLineCounter(void) const { return m_gridWideEvery; };

		//! \brief Sets the current GridSnapMode.
		//! \see GridSnapMode
		void setGridSnapMode(GridSnapMode _mode) { m_snapMode = _mode; };

		//! \brief Returns the current GridSnapMode.
		//! \see GridSnapMode
		GridSnapMode getGridSnapMode(void) const { return m_snapMode; };

		void setGridLineColor(int _r, int _g, int _b, int _a = 255) { this->setGridLineColor(Color(_r, _g, _b, _a)); };
		void setGridLineColor(const Color& _color) { m_lineStyle.setColor(_color); };
		void setGridLineStyle(const PenFCfg& _style) { m_lineStyle = _style; };
		const PenFCfg& getGridLineStyle(void) const { return m_lineStyle; };

		//! \brief Returns true if at least one type of grid should be drawn according to the current flags.
		bool isGridLinesValid(void) const { return (m_flags | Grid::NoGridLineMask) != NoGridLineMask; };

		//! \brief Returns true if the grid snap is enabled and the grid step is greater than 0 for at least one axis.
		bool isGridSnapValid(void) const { return (m_snapMode != NoGridSnap) && (m_gridStep.x() > 0 || m_gridStep.y() > 0); };

	private:
		GridFlags m_flags;
		Point2D m_gridStep;
		Point2D m_gridWideEvery;
		GridSnapMode m_snapMode;
		PenFCfg m_lineStyle;
	};
}
OT_ADD_FLAG_FUNCTIONS(ot::Grid::GridFlag)