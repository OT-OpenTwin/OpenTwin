#@ Port : {"type": "out", "name": "DataOutput", "label" : " Data Output"}
import json
import OpenTwin
import re

class Dataset:
    def __init__(_self, _selectedCampaignAsJsonDocument):
        _self.m_allSeries = _selectedCampaignAsJsonDocument["series"]
    
    def getEmptyQuantityDescription(_self):
        empty = {
            "Label": "",
            "Name": "",
            "Dimensions": [1],
            "TupleDescription": 
             {
                    "TupleTypeName": "",
                    "TupleFormatName": "",
                    "TupleElementDataTypes": "",
                    "TupleUnits": ""
             },
            "DependingParametersLabels": [""]
        }
    
        return empty

    def getEmptyParameterDescription(_self):
        empty = {
            "Label": "",
            "Name": "",
            "Type": "",
            "Unit": "",
            "Values": []
        }
        
        return empty
    
    def initiateNewSeries(_self, _seriesName):
        _self.m_newSeries = {
            "Label": "",
            "Name": _seriesName,
            "Metadata" : {},
            "quantities": [],
            "parameter": []
        }
        _self.m_data =[]

    def addQuantityDescription(_self,_quantityDescription):
        _self.m_newSeries["quantities"].append(_quantityDescription)

    def addParameterDescription(_self,_parameterDescription):
        _self.m_newSeries["parameter"].append(_parameterDescription)

    def setSeriesMetadata(_self, _metadata : dict):
        _self.m_newSeries["Metadata"] = _metadata

    def builtNewSeries(_self):
        _self.m_allSeries.append(_self.m_newSeries)

    def addDataPoint(_self ,_quantityName, _quantityValue, **_parameterKeyValuePairs):
        entry={
            _quantityName : _quantityValue
        }
        for key, value in _parameterKeyValuePairs.items():
            entry[key] = _quantityValue
        _self.m_data.append(entry)        



def parseFileContentMetadata_FormatA(lines: list[str]) -> tuple[dict | None, int]:
    split_idx = next(
        (i for i, l in enumerate(lines) if l.strip().startswith("Time,Ampl")),
        -1,
    )
    print("Here")
    print(split_idx)
    if split_idx == -1:
        return None, -1

    header_lines = lines[:split_idx]
    print("header")
    print(header_lines)
    dev_parts = [p.strip() for p in header_lines[0].split(",")]
    metadata = {
        "device_model": dev_parts[0] if len(dev_parts) > 0 else None,
        "device_id":    dev_parts[1] if len(dev_parts) > 1 else None,
        "record_type":  dev_parts[2] if len(dev_parts) > 2 else None,
    }

    for line in header_lines[1:]:
        parts = [p.strip() for p in line.split(",")]
        if not parts[0] or parts[0].startswith("#") or parts[0] == "Segment":
            break
        it = iter(parts)
        for key in it:
            val = next(it, None)
            if key and val is not None:
                try:    val = int(val)
                except ValueError:
                    try: val = float(val)
                    except ValueError: pass
                metadata[key] = val

    seg_header_idx = next(
        (i for i, l in enumerate(header_lines) if l.strip().startswith("Segment,")),
        -1,
    )
    if seg_header_idx != -1:
        segments = []
        for line in header_lines[seg_header_idx + 1:]:
            parts = [p.strip() for p in line.split(",")]
            if not parts[0].startswith("#"):
                break
            segments.append({
                "segment":           int(parts[0].lstrip("#")),
                "trig_time":         parts[1] if len(parts) > 1 else None,
                "time_since_seg1_s": float(parts[2]) if len(parts) > 2 else None,
            })
        if segments:
            metadata["segments"] = segments

    return metadata, split_idx

def _coerce(val: str):
    """Strip quotes and try numeric conversion."""
    val = val.strip().strip('"')
    if not val:
        return None
    try:    return int(val)
    except ValueError:
        try: return float(val)
        except ValueError: return val


def parseFileContentMetadata_FormatB(lines: list[str]) -> tuple[dict | None, list[tuple]]:
    """
    Parse a tab-separated LeCroy file where metadata (cols 0-2) and
    waveform data (cols 3-4) are interleaved on the same rows.

    Returns
    -------
    metadata : dict  — key/value pairs from the left columns
    data_rows: list of (time, amplitude) float tuples
    """
    metadata = {}
    data_rows = []
 
    for line in lines:
    
        parts = line.split("\t")

        # --- metadata columns (0-2): key, value, unit ---
        key_raw = parts[0].strip().strip('"') if len(parts) > 0 else ""
        val_raw = parts[1].strip()            if len(parts) > 1 else ""
        unit    = parts[2].strip().strip('"') if len(parts) > 2 else ""

        if key_raw:
            entry = {"value": _coerce(val_raw)}
            if unit:
                entry["unit"] = unit
            metadata[key_raw] = entry

        # --- data columns (3-4): time, amplitude ---
        if len(parts) >= 5:
            t_raw = parts[3].strip()
            a_raw = parts[4].strip()
            if t_raw and a_raw:
                try:
                    data_rows.append((float(t_raw), float(a_raw)))
                except ValueError:
                    pass  # skip malformed rows

    return metadata if metadata else None, data_rows



def extractMetadataFromFileName(filename: str) -> dict:
    
    stem = filename
    compact = stem.replace(" ", "").replace("_", "")

    pattern = re.compile(
        r"^(C\d)"
        r"(Vert|Hor)"
        r"Rad(\d+)"
        r"H(\d+)"
        r"Deg(\d{3})"
        r"(?:(Ref)(\d+)|(C)(\d+)|(\d+))$",
        re.IGNORECASE,
    )

    m = pattern.match(compact)
    if not m:
        raise ValueError(f"Unsupported filename format: {filename}")

    (
        channel,
        orientation,
        radius,
        height,
        degree,
        ref_tag,
        ref_segment,
        c_tag,
        c_segment,
        plain_segment,
    ) = m.groups()

    if ref_tag:
        reflector = True
        segment = int(ref_segment)
        segment_kind = "ref"
    elif c_tag:
        reflector = False
        segment = int(c_segment)
        segment_kind = "c"
    else:
        reflector = False
        segment = int(plain_segment)
        segment_kind = "plain"
    
    meta = {
        "channel": channel.lower(),
        "orientation": orientation.lower(),
        "radius_cm": int(radius),
        "height_cm": int(height),
        "degree": int(degree),
        "reflector": reflector,
        "segment": segment,
        "segment_kind": segment_kind,
        "filename": filename,
        "coord_system": "legacy",
    }
    return meta


def __main__(this):
    textFileName = "C1 Hor_Rad400_H008_Deg000_00000"
    textFileContent = OpenTwin.GetText("Text Files/" + textFileName + ".txt")
    lines = textFileContent.splitlines() 
   
    meta, dataTuples = parseFileContentMetadata_FormatB(lines)
    
    nameEncodedMetadata = extractMetadataFromFileName(textFileName)      
    
    combined_meta = {**meta, **nameEncodedMetadata}
   
    campaign =  {
        "Selected Campaign": {
            "Name": "Dataset/Campaign Metadata",
            "series": []
          }
    }

    newDataset = Dataset(campaign["Selected Campaign"])    
    quantity = newDataset.getEmptyQuantityDescription()
    quantity["Label"] = "Voltage"
    quantity["Name"] = "Voltage"
    quantity["TupleDescription"]["TupleUnits"] = ["V"]
    quantity["TupleDescription"]["TupleElementDataTypes"] = ["double"]

    parameter = newDataset.getEmptyParameterDescription()
    parameter["Label"] = "Time"
    parameter["Name"] = "Time"
    parameter["Unit"] = "s"
    parameter["Type"] = "double"

    newDataset.initiateNewSeries(textFileName)
    newDataset.setSeriesMetadata(combined_meta)
    print(newDataset.m_newSeries)
    data = [
        {"Time": t, "Values": v, "Quantity" : "Voltage"}
        for t, v in dataTuples
    ]

    newDataset.addParameterDescription(parameter)
    newDataset.addQuantityDescription(quantity)
    print("next")
    newDataset.builtNewSeries()
    print(campaign)
    OpenTwin.SetPortMetaData("DataOutput",json.dumps(campaign))
    OpenTwin.SetPortData("DataOutput",json.dumps(data))