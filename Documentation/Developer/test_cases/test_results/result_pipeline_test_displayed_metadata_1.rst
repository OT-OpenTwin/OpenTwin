:orphan:

.. _result_pipeline_test_displayed_metadata_1:

.. code-block:: json
    :caption: Displayed metadata in test 3.2
    
    {
        "Selected Campaign": {
            "Name": "Dataset/Campaign Metadata",
            "series": [
                {
                    "Label": "",
                    "Name": "Dataset/CMC_Stysc",
                    "quantities": [
                        {
                            "Label": "S-Parameter",
                            "Name": "S-Parameter",
                            "Dimensions": [4, 4],
                            "ValueDescriptions": [
                                {
                                    "Label": "Magnitude",
                                    "Name": "Magnitude", 
                                    "Type": "double",
                                    "Unit": ""
                                },
                                {
                                    "Label": "Phase",
                                    "Name": "Phase",
                                    "Type": "double", 
                                    "Unit": "Deg"
                                }
                            ],
                            "DependingParametersLabels": ["Frequency"]
                        }
                    ],
                    "parameter": [
                        {
                            "Label": "Frequency",
                            "Name": "Frequency",
                            "Type": "double",
                            "Unit": "Hz",
                            "Values": [9000.0, "[...]", 1000000000.0]
                        }
                    ]
                },
                {
                    "Label": "",
                    "Name": "Dataset/Magnitude > 1",
                    "quantities": [
                        {
                            "Label": "S-Parameter",
                            "Name": "S-Parameter",
                            "Dimensions": [4, 4],
                            "ValueDescriptions": [
                                {
                                    "Label": "Magnitude",
                                    "Name": "Magnitude",
                                    "Type": "double",
                                    "Unit": ""
                                },
                                {
                                    "Label": "Phase", 
                                    "Name": "Phase",
                                    "Type": "double",
                                    "Unit": "Deg"
                                }
                            ],
                            "DependingParametersLabels": ["Frequency_2"]
                        }
                    ],
                    "parameter": [
                        {
                            "Label": "Frequency",
                            "Name": "Frequency", 
                            "Type": "double",
                            "Unit": "Hz",
                            "Values": [
                                9000.0,
                                9105.174363669434,
                                9211.57779920257,
                                9319.224669582862
                            ]
                        }
                    ]
                }
            ]
        }
    }
