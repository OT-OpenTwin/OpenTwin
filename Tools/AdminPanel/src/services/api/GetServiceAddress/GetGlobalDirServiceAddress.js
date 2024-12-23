import msRequest from "../FesRequest/msRequest";

export function GetGlobalDirServiceAddress() {
  const data = {
    action: "Get.GlobalServicesURL",
    Msg: "UI.LogIn",
  };

  const sessionServiceAddress = "https://" + JSON.parse(sessionStorage.getItem("sessionservice")) + "/execute-one-way-tls";
  
  const request = msRequest(sessionServiceAddress, data);
  
  return request.then((value) => {
    let address = value["Service.GdsUrl"];
    return address;
  });
}

export default GetGlobalDirServiceAddress;
