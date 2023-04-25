import authRequest from "../../FesRequest/authRequest";

export function RegisterNewUser(props) {
  const data = {
    action: "Register",
    Username: props.enteredUsername,
    Password: props.enteredPassword,
  };

  return authRequest(data);
}

export default RegisterNewUser;
