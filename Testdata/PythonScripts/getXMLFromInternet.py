#@ Port : {"type": "out", "name": "FileOutput", "label" : "Euro foreign exchange reference rates"}
import requests
import OpenTwin
def __main__(this):
    # URL of the XML document
    url = "https://www.ecb.europa.eu/stats/eurofxref/eurofxref-daily.xml"

    # Send a GET request to the URL
    response = requests.get(url)

    # Check if the request was successful
    if response.status_code == 200:
        # Print the content of the XML
        xml_content = response.text

        # Wrap XML in a JSON object
        json_data = {"content": xml_content}
        OpenTwin.SetPortData("FileOutput",json.dumps(json_data))
    else:
        print(f"Failed to retrieve XML. Status code: {response.status_code}")