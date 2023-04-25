import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function GetAllGroups() {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "GetAllGroups",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
  };

  return authRequest(data);
}

export default GetAllGroups;
