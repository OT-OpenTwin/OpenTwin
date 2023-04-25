import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function GetAllUserCount() {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "GetAllUserCount",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
  };

  return authRequest(data);
}

export default GetAllUserCount;
