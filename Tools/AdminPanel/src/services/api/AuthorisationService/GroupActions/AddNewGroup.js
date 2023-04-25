import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function AddNewGroup(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "CreateGroup",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    GroupName: props.enteredGroupName,
  };

  return authRequest(data);
}

export default AddNewGroup;
