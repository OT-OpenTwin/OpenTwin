// @otlicense
// File: FileExtension.h
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

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTGui/GuiTypes.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT FileExtension {
		OT_DECL_STATICONLY(FileExtension)
	public:
		enum DefaultFileExtension {
			AllFiles,
			Unknown,

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
			Touchstone,       // .snp

			// Studio Suite / Simulation (CST-specific)
			CST,              // .cst
			CSTModel,         // .mod
			CSTLegacyProject, // .prj
			CSTFieldData,     // .fld
			CSTSignalData,    // .sig
			CSTSolverDb,      // .sdb
			CSTMaterial,      // .mcs
			CSTTemplate,      // .tcf

			// Mesh / Simulation Formats
			GmshMesh,         // .msh (Gmsh)
			Diffpack,         // .diff (Diffpack 3D)
			IDeasUniversal,   // .unv (I-deas Universal)
			Med,              // .med (MED)
			INRIAMedit,       // .mesh (INRIA Medit)
			NastranBulkData,  // .bdf (Nastran Bulk Data)
			Plot3d,           // .p3d (Plot3D)
			STLSurface,       // .stl (STL Surface)
			VRMLSurface,      // .wrl (VRML)
			Vtk,              // .vtk (VTK)
			Ply2Surface       // .ply2 (PLY2 Surface)
		};

		//! @brief Creates a filter string that may be used in a file dialog.
		//! @param _extensions Extensions to convert.
		static std::string toFilterString(const std::list<DefaultFileExtension>& _extensions);

		//! @brief Creates a filter string that may be used in a file dialog.
		//! @param _extensions Extensions to convert.
		static std::string toFilterString(const std::initializer_list<DefaultFileExtension>& _extensions);

		//! @brief Creates a filter string that may be used in a file dialog.
		//! @param _extension Extension to convert.
		static std::string toFilterString(DefaultFileExtension _extension);
		
		//! @brief Converts a file extension string (without dot) to the corresponding DefaultFileExtension enum value.
		static std::string toString(DefaultFileExtension _extension);

		//! @brief Converts a file extension string (without dot) to the corresponding DefaultFileExtension enum value.
		static DefaultFileExtension stringToFileExtension(const std::string& _extension);

		//! @brief Converts a DefaultFileExtension to the corresponding ImageFileFormat.
		//! @param _extension Extension to convert.
		//! @param _success Returns true if the conversion was successful, false otherwise.
		static ImageFileFormat toImageFileFormat(DefaultFileExtension _extension, bool& _success);

		//! @brief Converts an ImageFileFormat to the corresponding DefaultFileExtension.
		//! @param _format Format to convert.
		//! @return Corresponding DefaultFileExtension. If no corresponding extension exists, DefaultFileExtension::Unknown is returned.
		static DefaultFileExtension fromImageFileFormat(ImageFileFormat _format);

	};

}