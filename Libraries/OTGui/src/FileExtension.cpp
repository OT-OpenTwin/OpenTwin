//! @file FileExtension.cpp
//! @author Alexander Kuester (alexk95)
//! @date July 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/FileExtension.h"

std::string ot::FileExtension::toFilterString(const std::list<DefaultFileExtension>& _extensions) {
	std::string result;

	for (DefaultFileExtension ext : _extensions) {
		if (!result.empty()) {
			result.append(";;");
		}
		result.append(toFilterString(ext));
	}

	if (result.empty()) {
		result = toFilterString(FileExtension::AllFiles);
	}

	return result;
}

std::string ot::FileExtension::toFilterString(const std::initializer_list<DefaultFileExtension>& _extensions) {
	return toFilterString(std::list<DefaultFileExtension>(_extensions));
}


std::string ot::FileExtension::toFilterString(DefaultFileExtension _extension) {
	switch (_extension) {
	case FileExtension::AllFiles: return "All Files (*.*)";

		// Text / Script
	case FileExtension::Text: return "Text Files (*.txt)";
	case FileExtension::CSV: return "Comma-Separated Values (*.csv)";
	case FileExtension::Python: return "Python Scripts (*.py)";
	case FileExtension::JSON: return "JSON Files (*.json)";
	case FileExtension::XML: return "XML Files (*.xml)";
	case FileExtension::YAML: return "YAML Files (*.yaml *.yml)";
	case FileExtension::Markdown: return "Markdown (*.md)";
	case FileExtension::INI: return "INI Config (*.ini)";
	case FileExtension::Log: return "Log Files (*.log)";
	case FileExtension::HTML: return "HTML Files (*.html *.htm)";
	case FileExtension::JavaScript: return "JavaScript (*.js)";
	case FileExtension::Batch: return "Batch Files (*.bat)";
	case FileExtension::Shell: return "Shell Scripts (*.sh)";
	case FileExtension::Cpp: return "C++ Files (*.cpp *.h *.hpp)";
	case FileExtension::CppSource: return "C++ Source (*.cpp)";
	case FileExtension::CppHeader: return "C++ Header (*.h *.hpp)";
	case FileExtension::CSharp: return "C# Source (*.cs)";
	case FileExtension::Java: return "Java Source (*.java)";

		// Image
	case FileExtension::Png: return "PNG Image (*.png)";
	case FileExtension::Ico: return "Icon (*.ico)";
	case FileExtension::Jpeg: return "JPEG Image (*.jpg *.jpeg *.jpe *.jiff)";
	case FileExtension::Gif: return "GIF Image (*.gif)";
	case FileExtension::Heic: return "HEIC Image (*.heic)";
	case FileExtension::Bitmap: return "Bitmap (*.bmp *.dib)";
	case FileExtension::Tiff: return "TIFF Image (*.tif *.tiff)";
	case FileExtension::Webp: return "WebP Image (*.webp)";
	case FileExtension::Svg: return "SVG Image (*.svg)";
	case FileExtension::Psd: return "Photoshop Document (*.psd)";

		// Office / Documents
	case FileExtension::PDF: return "PDF Documents (*.pdf)";
	case FileExtension::MSWord: return "Word Documents (*.doc *.docx)";
	case FileExtension::MSExcel: return "Excel Spreadsheets (*.xls *.xlsx)";
	case FileExtension::MSPowerPoint: return "PowerPoint Presentations (*.ppt *.pptx)";
	case FileExtension::Rtf: return "Rich Text Format (*.rtf)";

		// Archives
	case FileExtension::Zip: return "ZIP Archive (*.zip)";
	case FileExtension::Rar: return "RAR Archive (*.rar)";
	case FileExtension::SevenZip: return "7-Zip Archive (*.7z)";
	case FileExtension::Tar: return "TAR Archive (*.tar)";
	case FileExtension::Gz: return "GZip Archive (*.gz)";

		// Technical / Engineering
	case FileExtension::Step: return "STEP Files (*.step *.stp)";
	case FileExtension::Iges: return "IGES Files (*.iges *.igs)";
	case FileExtension::Obj: return "Wavefront OBJ (*.obj)";
	case FileExtension::Dxf: return "DXF Drawings (*.dxf)";
	case FileExtension::Dwg: return "DWG Drawings (*.dwg)";
	case FileExtension::Sldprt: return "SolidWorks Part (*.sldprt)";
	case FileExtension::Sldasm: return "SolidWorks Assembly (*.sldasm)";
	case FileExtension::F3d: return "Fusion 360 Files (*.f3d)";
	case FileExtension::Fbx: return "FBX 3D (*.fbx)";
	case FileExtension::Glb: return "glTF Binary (*.glb)";
	case FileExtension::Gltf: return "glTF Text (*.gltf)";
	case FileExtension::Amf: return "Additive Manufacturing Format (*.amf)";
	case FileExtension::Brd: return "EAGLE Board (*.brd)";
	case FileExtension::Sch: return "EAGLE Schematic (*.sch)";
	case FileExtension::Spice: return "SPICE Files (*.cir *.sp)";
	case FileExtension::Mat: return "MATLAB Data Files (*.mat)";
	case FileExtension::Mdl: return "Simulink Model (*.mdl)";
	case FileExtension::Touchstone: return "Touchstone files (*.s*p)";

		// CST Studio Suite
	case FileExtension::CST: return "CST Project (*.cst)";
	case FileExtension::CSTModel: return "CST Model (*.mod)";
	case FileExtension::CSTLegacyProject: return "CST Legacy Project (*.prj)";
	case FileExtension::CSTFieldData: return "CST Field Data (*.fld)";
	case FileExtension::CSTSignalData: return "CST Signal (*.sig)";
	case FileExtension::CSTSolverDb: return "CST Solver DB (*.sdb)";
	case FileExtension::CSTMaterial: return "CST Material Catalog (*.mcs)";
	case FileExtension::CSTTemplate: return "CST Template Control (*.tcf)";

		// Mesh / Simulation Formats
	case GmshMesh: return "Gmsh MSH (*.msh)";
	case Diffpack: return "Diffpack 3D (*.diff)";
	case IDeasUniversal: return "I-deas Universal (*.unv)";
	case Med: return "MED (*.med)";
	case INRIAMedit: return "INRIA Medit (*.mesh)";
	case NastranBulkData: return "Nastran Bulk Data File (*.bdf)";
	case Plot3d: return "Plot3D Structured Mesh (*.p3d)";
	case STLSurface: return "STL Surface (*.stl)"; // already exists
	case VRMLSurface: return "VRML Surface (*.wrl)";
	case Vtk: return "VTK (*.vtk)";
	case Ply2Surface: return "PLY2 Surface (*.ply2)";

	default:
		OT_LOG_E("Unknown file extension (" + std::to_string(static_cast<int>(_extension)) + ")");
		return "";
	}
}

