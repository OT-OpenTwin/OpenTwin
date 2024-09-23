/* eslint-disable react-hooks/exhaustive-deps */
import "./LandingPage.scss";
import React, { useState, useEffect } from "react";
import { Link, useNavigate } from "react-router-dom";
import { useTranslation } from "react-i18next";

import Navbar from "../../components/Navbar/Navbar";
import Card from "../../components/Card/Card";

import userIcon from "../../resources/icons/users-icon.png";
import groupIcon from "../../resources/icons/groups-icon.png";
import projectIcon from "../../resources/icons/project-icon.png";
import dashboardIcon from "../../resources/icons/dashboard-icon.png";

import GetAllUserCount from "../../services/api/AuthorisationService/UserActions/GetAllUserCount";
import GetAllGroupCount from "../../services/api/AuthorisationService/GroupActions/GetAllGroupCount";
import GetAllProjectCount from "../../services/api/AuthorisationService/ProjectActions/GetAllProjectCount";

const LandingPage = () => {
  const navigate = useNavigate();
  const [numberOfUsers, setNumberOfUsers] = useState(null);
  const [numberOfGroups, setNumberOfGroups] = useState(null);
  const [numberOfProjects, setNumberOfProjects] = useState(null);

  useEffect(() => {
    GetAllUserCount()
      .then((response) => {
        const number = response;
        setNumberOfUsers(number);
        GetAllGroupCount()
          .then((response) => {
            const number = response;
            setNumberOfGroups(number);
            GetAllProjectCount()
              .then((response) => {
                const number = response;
                setNumberOfProjects(number);
              })
              .catch((response) => {
                console.log("GetAllProjects", response.description);
                navigate("/error");
              });
          })
          .catch((response) => {
            console.log("GetAllGroups", response.description);
            navigate("/error");
          });
      })
      .catch((response) => {
        console.log("GetAllUsers", response.description);
        navigate("/error");
      });
  }, []);

  const { t } = useTranslation();
  return (
    <div className="landingPage">
      <Navbar />
      <div className="homeContainer">
        {" "}
        <Link to="/users" style={{ textDecoration: "none" }}>
          <Card
            title={t("landingPage:userTitle")}
            number={numberOfUsers}
            linkTo={t("landingPage:userButton")}
            icon={userIcon}
          />
        </Link>
      </div>
      <div className="homeContainer">
        <Link to="/groups" style={{ textDecoration: "none" }}>
          <Card
            title={t("landingPage:groupTitle")}
            number={numberOfGroups}
            linkTo={t("landingPage:groupButton")}
            icon={groupIcon}
          />
        </Link>
      </div>
      <div className="homeContainer">
        <Link to="/projects" style={{ textDecoration: "none" }}>
          <Card
            title={t("landingPage:projectTitle")}
            number={numberOfProjects}
            linkTo={t("landingPage:projectButton")}
            icon={projectIcon}
          />
        </Link>
      </div>
      <div className="homeContainer">
        <Link to="/dashboard" style={{ textDecoration: "none" }}>
          <Card
            title="Dashboard"
            number="WIP"
            linkTo={t("landingPage:projectButton")}
            icon={dashboardIcon}
          />
        </Link>
      </div>
    </div>
  );
};

export default LandingPage;
