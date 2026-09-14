#@ Port : {"type": "in", "name": "FileName", "label" : "Data Input"}
#@ Port : {"type": "out", "name": "DataOutput", "label" : " Data Output"}
import json
import OpenTwin

def parseFileContentMetadata_FormatA(lines: list[str]) -> tuple[dict | None, int]:
    split_idx = next(
        (i for i, l in enumerate(lines) if l.strip().startswith("Time,Ampl")),
        -1,
    )
    if split_idx == -1:
        return None, -1

    header_lines = lines[:split_idx]

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

def extractMetadataFromFileName(filename: str) -> dict:
    base = os.path.basename(filename)
    stem = base.rsplit(".", 1)[0]
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

    return {
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


def main():

    textFileContent = list(str)
    textFileName = str
    meta, split_idx = parseFileContentMetadata_FormatA(textFileContent)
    nameEncodedMetadata = extractMetadataFromFileName(textFileName)
    dataPoints = textFileContent[split_idx:]
    combined_meta = {**meta, **nameEncodedMetadata}
    print(combined_meta)
    print("Now the data \n\n")
    print(dataPoints)