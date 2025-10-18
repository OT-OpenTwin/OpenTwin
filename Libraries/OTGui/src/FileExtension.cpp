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
	case FileExtension::AllFiles:
	case FileExtension::Unknown: return "All Files (*.*)";

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

std::string ot::FileExtension::toString(DefaultFileExtension _extension) {
	switch (_extension) {
	case AllFiles: return "*";
	case Unknown: return "";

		// Text / Script

	case Text: return "txt";
	case CSV: return "csv";
	case Python: return "py";
	case JSON: return "json";
	case XML: return "xml";
	case YAML: return "yaml";
	case Markdown: return "md";
	case INI: return "ini";
	case Log: return "log";
	case HTML: return "html";
	case JavaScript: return "js";
	case Batch: return "bat";
	case Shell: return "sh";
	case Cpp: return "cpp";
	case CppSource: return "cpp";
	case CppHeader: return "h";
	case CSharp: return "cs";
	case Java: return "java";

		// Image

	case Png: return "png";
	case Ico: return "ico";
	case Jpeg: return "jpg";
	case Gif: return "gif";
	case Heic: return "heic";
	case Bitmap: return "bmp";
	case Tiff: return "tif";
	case Webp: return "webp";
	case Svg: return "svg";
	case Psd: return "psd";

		// Office / Documents

	case PDF: return "pdf";
	case MSWord: return "docx";
	case MSExcel: return "xlsx";
	case MSPowerPoint: return "pptx";
	case Rtf: return "rtf";

		// Archives

	case Zip: return "zip";
	case Rar: return "rar";
	case SevenZip: return "7z";
	case Tar: return "tar";
	case Gz: return "gz";

		// Technical / Engineering

	case Step: return "step";
	case Iges: return "iges";
	case Obj: return "obj";
	case Dxf: return "dxf";
	case Dwg: return "dwg";
	case Sldprt: return "sldprt";
	case Sldasm: return "sldasm";
	case F3d: return "f3d";
	case Fbx: return "fbx";
	case Glb: return "glb";
	case Gltf: return "gltf";
	case Amf: return "amf";
	case Brd: return "brd";
	case Sch: return "sch";
	case Spice: return "cir";
	case Mat: return "mat";
	case Mdl: return "mdl";
	case Touchstone: 
		OT_LOG_W("Touchstone file extension is ambiguous");
		return "s*p";

		// Studio Suite / Simulation (CST-specific)

	case CST: return "cst";
	case CSTModel: return "mod";
	case CSTLegacyProject: return "prj";
	case CSTFieldData: return "fld";
	case CSTSignalData: return "sig";
	case CSTSolverDb: return "sdb";
	case CSTMaterial: return "mcs";
	case CSTTemplate: return "tcf";

		// Mesh / Simulation Formats

	case GmshMesh: return "msh";
	case Diffpack: return "diff";
	case IDeasUniversal: return "unv";
	case Med: return "med";
	case INRIAMedit: return "mesh";
	case NastranBulkData: return "bdf";
	case Plot3d: return "p3d";
	case STLSurface: return "stl";
	case VRMLSurface: return "wrl";
	case Vtk: return "vtk";
	case Ply2Surface: return "ply2";
	default: 
		OT_LOG_E("Unknown file extension (" + std::to_string(static_cast<int>(_extension)) + ")");
		return "";
	}
}

ot::FileExtension::DefaultFileExtension ot::FileExtension::stringToFileExtension(const std::string& _extension) {
	// Text / Script

	if (_extension == "txt") return Text;
	if (_extension == "csv") return CSV;
	if (_extension == "py") return Python;
	if (_extension == "json") return JSON;
	if (_extension == "xml") return XML;
	if (_extension == "yaml" || _extension == "yml") return YAML;
	if (_extension == "md") return Markdown;
	if (_extension == "ini") return INI;
	if (_extension == "log") return Log;
	if (_extension == "html" || _extension == "htm") return HTML;
	if (_extension == "js") return JavaScript;
	if (_extension == "bat") return Batch;
	if (_extension == "sh") return Shell;
	if (_extension == "cpp" || _extension == "cc" || _extension == "cxx") return CppSource;
	if (_extension == "h" || _extension == "hpp" || _extension == "hh") return CppHeader;
	if (_extension == "cs") return CSharp;
	if (_extension == "java") return Java;

	// Image

	if (_extension == "png") return Png;
	if (_extension == "ico") return Ico;
	if (_extension == "jpg" || _extension == "jpeg" || _extension == "jpe" || _extension == "jiff") return Jpeg;
	if (_extension == "gif") return Gif;
	if (_extension == "heic") return Heic;
	if (_extension == "bmp") return Bitmap;
	if (_extension == "tif" || _extension == "tiff") return Tiff;
	if (_extension == "webp") return Webp;
	if (_extension == "svg") return Svg;
	if (_extension == "psd") return Psd;

	// Office / Documents

	if (_extension == "pdf") return PDF;
	if (_extension == "doc" || _extension == "docx") return MSWord;
	if (_extension == "xls" || _extension == "xlsx") return MSExcel;
	if (_extension == "ppt" || _extension == "pptx") return MSPowerPoint;
	if (_extension == "rtf") return Rtf;

	// Archives

	if (_extension == "zip") return Zip;
	if (_extension == "rar") return Rar;
	if (_extension == "7z") return SevenZip;
	if (_extension == "tar") return Tar;
	if (_extension == "gz") return Gz;

	// Technical / Engineering

	if (_extension == "step" || _extension == "stp") return Step;
	if (_extension == "iges" || _extension == "igs") return Iges;
	if (_extension == "obj") return Obj;
	if (_extension == "dxf") return Dxf;
	if (_extension == "dwg") return Dwg;
	if (_extension == "sldprt") return Sldprt;
	if (_extension == "sldasm") return Sldasm;
	if (_extension == "f3d") return F3d;
	if (_extension == "fbx") return Fbx;
	if (_extension == "glb") return Glb;
	if (_extension == "gltf") return Gltf;
	if (_extension == "amf") return Amf;
	if (_extension == "brd") return Brd;
	if (_extension == "sch") return Sch;
	if (_extension == "cir" || _extension == "sp") return Spice;
	if (_extension == "mat") return Mat;
	if (_extension == "mdl") return Mdl;

	// Studio Suite / Simulation (CST-specific)

	if (_extension == "cst") return CST;
	if (_extension == "mod") return CSTModel;
	if (_extension == "prj") return CSTLegacyProject;
	if (_extension == "fld") return CSTFieldData;
	if (_extension == "sig") return CSTSignalData;
	if (_extension == "sdb") return CSTSolverDb;
	if (_extension == "mcs") return CSTMaterial;
	if (_extension == "tcf") return CSTTemplate;

	// Mesh / Simulation Formats

	if (_extension == "msh") return GmshMesh;
	if (_extension == "diff") return Diffpack;
	if (_extension == "unv") return IDeasUniversal;
	if (_extension == "med") return Med;
	if (_extension == "mesh") return INRIAMedit;
	if (_extension == "bdf") return NastranBulkData;
	if (_extension == "p3d") return Plot3d;
	if (_extension == "stl") return STLSurface;
	if (_extension == "wrl") return VRMLSurface;
	if (_extension == "vtk") return Vtk;
	if (_extension == "ply2") return Ply2Surface;

	return Unknown;
}

ot::ImageFileFormat ot::FileExtension::toImageFileFormat(DefaultFileExtension _extension, bool& _success) {
	_success = true;
	switch (_extension) {
	case Png: return ot::ImageFileFormat::PNG;
	case Jpeg: return ot::ImageFileFormat::JPEG;
	case Svg: return ot::ImageFileFormat::SVG;
	default:
		_success = false;
		return ot::ImageFileFormat::PNG;
	}
}
