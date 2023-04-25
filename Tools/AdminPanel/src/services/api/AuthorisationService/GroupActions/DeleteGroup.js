import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function DeleteGroup(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "RemoveGroup",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    GroupName: props.groupNameToRemove,
  };

  return authRequest(data);
}

export default DeleteGroup;
