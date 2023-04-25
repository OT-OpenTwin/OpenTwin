import fesRequest from "../../FesRequest/fesRequest";
import { getUserData } from "../../../../screens/LoginPage/LoginPage";

export function GetAllProjectCount() {
  const userData = getUserData();
  const enteredUsername = userData[0];
  const enteredPassword = userData[1];
  const data = {
    action: "GetAllProjectCount",
    LoggedInUsername: enteredUsername,
    LoggedInUserPassword: enteredPassword,
  };

  return new Promise((resolve) => {
    fesRequest({
      method: "POST",
      mode: "no-cors",
      headers: { "content-type": "application/json" },
      data: JSON.stringify(data),
      url: "https://127.0.0.1:8092/execute-one-way-tls",
    })
      .then((res) => {
        if (res.data === 0) {
          return resolve(res.data);
        }
        return resolve(res);
      })
      .catch((err) => {
        resolve(err);
      });
  });
}

export default GetAllProjectCount;
