import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function GetAllUsers() {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "GetAllUsers",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
  };

  return authRequest(data);
}

export default GetAllUsers;
