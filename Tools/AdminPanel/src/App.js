import { useState } from "react";
import { UserContextProvider } from "./components/context/userDetails";
import { BrowserRouter, Routes, Route } from "react-router-dom";

import LoginPage from "./screens/LoginPage/LoginPage";

import LandingPage from "./screens/LandingPage/LandingPage";

import DashboardPage from "./screens/DashboardPage/DashboardPage";

import UserPage from "./screens/UserPage/UserPage";
import CreateNewUser from "./screens/UserPage/CreateNewUser";
import ChangeUserUsername from "./screens/UserPage/ChangeUsername";
import ChangeUserPassword from "./screens/UserPage/ChangePassword";

import GroupPage from "./screens/GroupPage/GroupPage";
import ChangeGroupName from "./screens/GroupPage/ChangeGroupName";

import ProjectPage from "./screens/ProjectPage/ProjectPage";
import ChangeProjectName from "./screens/ProjectPage/ChangeProjectName";

import ErrorPage from "./screens/ErrorPage/ErrorPage";

import StickyFooter from "../src/components/Footer/Footer";

export function setToken(userToken) {
  sessionStorage.setItem("loggedIn", JSON.stringify(userToken));
}

function getToken() {
  const tokenString = sessionStorage.getItem("loggedIn");
  const userToken = JSON.parse(tokenString);
  return userToken;
}

function App() {
  const [isLoggedIn, setLoggedIn] = useState(getToken);

  return (
    <div>
      <UserContextProvider>
        <BrowserRouter>
          <Routes>
            <Route path="/">
              <Route
                index
                element={
                  <LoginPage setLoggedIn={setLoggedIn} setToken={setToken} />
                }
              ></Route>
              {isLoggedIn && (
                <Route path="landingPage" element={<LandingPage />}></Route>
              )}
              {isLoggedIn && (
                <Route path="dashboard" element={<DashboardPage />}></Route>
              )}
              {isLoggedIn && (
                <Route path="users">
                  <Route index element={<UserPage />} />
                  <Route
                    path="changeUsername"
                    element={<ChangeUserUsername />}
                  />
                  <Route
                    path="changePassword"
                    element={<ChangeUserPassword />}
                  />
                  <Route
                    path="new"
                    element={<CreateNewUser title="Add New User" />}
                  />
                </Route>
              )}
              {isLoggedIn && (
                <Route path="groups">
                  <Route index element={<GroupPage />} />
                  <Route path=":groupId" element={<ChangeGroupName />} />
                </Route>
              )}
              {isLoggedIn && (
                <Route path="projects">
                  <Route index element={<ProjectPage />} />
                  <Route path=":projectId" element={<ChangeProjectName />} />
                </Route>
              )}
              {isLoggedIn && (
                <Route path="error">
                  <Route index element={<ErrorPage />} />
                </Route>
              )}
              {!isLoggedIn && (
                <Route
                  path="*"
                  element={
                    <LoginPage setLoggedIn={setLoggedIn} setToken={setToken} />
                  }
                />
              )}
            </Route>
          </Routes>
        </BrowserRouter>
        <div>
          <StickyFooter />
        </div>
      </UserContextProvider>
    </div>
  );
}

export default App;
