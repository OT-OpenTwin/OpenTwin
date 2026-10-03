#@ Event : {"name": "Table Filter Changed", "function": "FilterChanged"}
# Possible events: "Item Deleted", "Task Started", "Task Finished", "Table Filter Changed", "Property Updated"

import OpenTwin 
import json

def getAllCurveNames():
    recursiveSearch = True
    folderContent = OpenTwin.GetFolderItemNames("Plots", recursiveSearch)
    curveNames  = list()
    for entity in folderContent:
        level = entity.count("/") # Determines the level of the entity. Structure is like this: Plots/Plot/Curve. Thus "Plot" has level 1 and "Curve" has level 2
        if(level == 2):
            curveNames.append(entity)
    return curveNames

def getAllSeriesMetadataPropertyGroups(aCurveName):
    propertyGroups = OpenTwin.GetPropertyGroups(aCurveName)
    relevantGroupNameBase = "Series metadata"
    metadataGroups = [groupName for groupName in propertyGroups if relevantGroupNameBase in groupName]
    return metadataGroups

def mergeComparisons(comparisons):
    merged = {}

    for comparison in comparisons:
        name = comparison.name

        if name not in merged:
            merged[name] = {
                "name": name,
                "values": [],
                "comparator": comparison.comparator
            }

        merged[name]["values"].append(str(comparison.value))

    result = []

    for item in merged.values():
        if len(item["values"]) > 1:
            comparator = "Any of"
        else:
            comparator = item["comparator"]

        result.append({
            "name": item["name"],
            "value": ",".join(item["values"]),
            "comparator": comparator
        })

    return result

def setAllFilter(curveNames, propertyGroups, comparisons):
    success = True

    # Merge repeated filter names before writing them to the curve
    mergedComparisons = mergeComparisons(comparisons)

    for curveName in curveNames:
        # Reset existing filter properties
        for propertyGroup in propertyGroups:
            success &= OpenTwin.SetPropertyValue(
                curveName, "Name", "", propertyGroup
            )
            success &= OpenTwin.SetPropertyValue(
                curveName, "Value", "", propertyGroup
            )
            success &= OpenTwin.SetPropertyValue(
                curveName, "Comparator", "", propertyGroup
            )

        if not success:
            raise Exception(
                "Resetting the filter properties was not successful"
            )

        # Only use as many groups as are available
        limit = min(len(mergedComparisons), len(propertyGroups))

        for i in range(limit):
            currentGroup = propertyGroups[i]
            currentFilter = mergedComparisons[i]

            success &= OpenTwin.SetPropertyValue(
                curveName,
                "Name",
                currentFilter["name"],
                currentGroup
            )
            success &= OpenTwin.SetPropertyValue(
                curveName,
                "Value",
                currentFilter["value"],
                currentGroup
            )
            success &= OpenTwin.SetPropertyValue(
                curveName,
                "Comparator",
                currentFilter["comparator"],
                currentGroup
            )

        if not success:
            raise Exception(
                "Setting new filter properties was not successful"
            )

        success &= OpenTwin.SetPropertyValue(
            curveName,
            "Number of metadata queries",
            limit,
        )

        if not success:
            raise Exception(
                "Setting the number of metadata queries was not successful"
            )

def FilterChanged(this, **kwargs):
    print("Handle table filter changed event. Setting filter:")
    comparisons = kwargs.get("comparisons", [])
    print(comparisons)
    curveNames = getAllCurveNames()
    if(len(curveNames) > 0):
        print("Performing update on curves: ")
        print(curveNames)
        propertyGroups = getAllSeriesMetadataPropertyGroups(curveNames[0])
        if len(comparisons) > len(propertyGroups):
            print("The curve entity does not support as many filters as are requested. All possible filters are being set.")
        setAllFilter(curveNames,propertyGroups,comparisons)
        OpenTwin.Flush()
        print("Curve update done.")
    else:
        print("No curve found that could be updated.")
    
        
	

def __main__(self, **kwargs):
    # Works with no kwargs — kwargs is just an empty dict {}
    # Works with kwargs  — kwargs["measurements"], kwargs["scale"], etc.
    print("Main")