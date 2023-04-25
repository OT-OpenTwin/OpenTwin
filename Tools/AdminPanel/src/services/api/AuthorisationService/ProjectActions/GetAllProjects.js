import authRequest from "../../FesRequest/authRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function GetAllProjects(props) {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "GetAllProjects",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
    ProjectFilter: props.enteredFilterValue,
    ProjectLimit: 10,
  };

  return authRequest(data);
}

export default GetAllProjects;
