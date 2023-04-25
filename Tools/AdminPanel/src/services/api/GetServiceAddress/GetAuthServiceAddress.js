import msRequest from "../FesRequest/msRequest";

export function GetAuthServiceAddress() {
  const data = {
    action: "Get.DatabaseAuthURL",
    Msg: "UI.LogIn",
  };

  const sessionServiceAddress = "https://" + JSON.parse(sessionStorage.getItem("sessionservice")) + "/execute-one-way-tls";
  
  const request = msRequest(sessionServiceAddress, data);
  
  return request.then((value) => {
    let address = value["Service.AuthUrl"];
    return address;
  });
}

export default GetAuthServiceAddress;
