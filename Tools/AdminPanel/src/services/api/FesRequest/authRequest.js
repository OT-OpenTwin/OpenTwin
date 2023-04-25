import msRequest from "./msRequest";

export function authRequest(data) {

    const authServiceAddress = "https://" + sessionStorage.getItem("authServiceAddress") + "/execute-one-way-tls";
    return msRequest(authServiceAddress, data);
}

export default authRequest;
