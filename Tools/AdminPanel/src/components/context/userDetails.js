import { createContext, useState } from "react";

export const UserContext = createContext(undefined);

export const UserContextProvider = (props) => {
  const [userDetails, setUserDetails] = useState("");

  return (
    <UserContext.Provider value={{ userDetails, setUserDetails }}>
      {props.children}
    </UserContext.Provider>
  );
};
