//! @file FileExtension.h
//! @author Alexander Kuester (alexk95)
//! @date July 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT FileExtension {
		OT_DECL_STATICONLY(FileExtension)
	public:
		enum DefaultFileExtension {
			AllFiles,

			// Text / Script

			Text,             // .txt
			CSV,              // .csv
			Python,           // .py
			JSON,             // .json
			XML,              // .xml
			YAML,             // .yaml / .yml
			Markdown,         // .md
			INI,              // .ini
			Log,              // .log
			HTML,             // .html / .htm
			JavaScript,       // .js
			Batch,            // .bat
			Shell,            // .sh
			Cpp,              // .cpp / .h / .hpp
			CppSource,        // .cpp
			CppHeader,        // .h / .hpp
			CSharp,           // .cs
			Java,             // .java

			// Image
			Png,              // .png
			Ico,              // .ico
			Jpeg,             // .jpg / .jpeg / .jpe / .jiff
			Gif,              // .gif
			Heic,             // .heic
			Bitmap,           // .bmp
			Tiff,             // .tif / .tiff
			Webp,             // .webp
			Svg,              // .svg
			Psd,              // .psd

			// Office / Documents
			PDF,              // .pdf
			MSWord,           // .doc / .docx
			MSExcel,          // .xls / .xlsx
			MSPowerPoint,     // .ppt / .pptx
			Rtf,              // .rtf

			// Archives
			Zip,              // .zip
			Rar,              // .rar
			SevenZip,         // .7z
			Tar,              // .tar
			Gz,               // .gz

			// Technical / Engineering
			Step,             // .step / .stp
			Iges,             // .iges / .igs
			Stl,              // .stl
			Obj,              // .obj
			Dxf,              // .dxf
			Dwg,              // .dwg
			Sldprt,           // .sldprt (SolidWorks Part)
			Sldasm,           // .sldasm (SolidWorks Assembly)
			F3d,              // .f3d (Fusion 360)
			Fbx,              // .fbx
			Glb,              // .glb
			Gltf,             // .gltf
			Amf,              // .amf (Additive Manufacturing File Format)
			Brd,              // .brd (EAGLE board file)
			Sch,              // .sch (EAGLE schematic)
			Spice,            // .cir / .sp (SPICE simulation files)
			Mat,              // .mat (MATLAB data)
			Mdl,              // .mdl (Simulink models)

			// Studio Suite / Simulation (CST-specific)
			CST,              // .cst
			CSTModel,         // .mod
			CSTLegacyProject, // .prj
			CSTFieldData,     // .fld
			CSTSignalData,    // .sig
			CSTSolverDb,      // .sdb
			CSTMaterial,      // .mcs
			CSTTemplate,      // .tcf
		};

		//! @brief Creates a filter string that may be used in a file dialog.
		//! @param _extensions Extensions to convert.
		static std::string toFilterString(const std::list<DefaultFileExtension>& _extensions);

		//! @brief Creates a filter string that may be used in a file dialog.
		//! @param _extension Extension to convert.
		static std::string toFilterString(DefaultFileExtension _extension);
		
	};

}