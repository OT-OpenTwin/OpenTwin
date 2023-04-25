import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function GetAllGroupCount() {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "GetAllGroupCount",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
  };

  return authRequest(data);
}

export default GetAllGroupCount;
