import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function ChangeGroupOwner(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "ChangeGroupOwner",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    GroupName: props.grpName,
    NewGroupOwnerUsername: props.ownerUsername,
  };

  return authRequest(data);
}

export default ChangeGroupOwner;
