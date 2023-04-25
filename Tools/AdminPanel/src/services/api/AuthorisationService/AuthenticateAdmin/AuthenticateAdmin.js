import authRequest from "../../FesRequest/authRequest";

export function AuthenticateAdmin(props) {
  const data = {
    action: "LoginAdmin",
    Username: props.enteredUsername,
    Password: props.enteredPassword,
    EncryptedPassword: false,
  };
  
  return authRequest(data);
}

export default AuthenticateAdmin;
