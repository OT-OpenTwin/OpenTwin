import msRequest from "../FesRequest/msRequest";

export function GetAuthServiceAddress() {
  const data = {
    action: "Get.GlobalServicesURL",
    Msg: "UI.LogIn",
  };

  const sessionServiceAddress = "https://" + JSON.parse(sessionStorage.getItem("sessionservice")) + "/execute-one-way-tls";
  
  const request = msRequest(sessionServiceAddress, data);
  
  return request.then((value) => {
    const whatData = JSON.parse(value.What);
    let address = whatData["Service.AuthUrl"];
    return address;
  });
}

export default GetAuthServiceAddress;
