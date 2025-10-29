/* eslint-disable react-hooks/exhaustive-deps */
import "./style/ProjectPage.scss";
import React, { useState, useEffect, useRef } from "react";
import { useTranslation } from "react-i18next";
import { useNavigate } from "react-router-dom";

import { Sidebar } from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import DataGrid from "../../components/DataGrid/DataGrid";
import ChangeOwnerModal from "../../components/Modal/ChangeOwnerModal/ChangeOwnerModal";
import InfoModal from "../../components/Modal/InfoModal/InfoModal";
import { CircularProgress as Spinner } from "@mui/material";

import { ProjectsColumnHeader } from "../../resources/dataGridSources";
import GetAllProjects from "../../services/api/AuthorisationService/ProjectActions/GetAllProjects";
import GetAllUsers from "../../services/api/AuthorisationService/UserActions/GetAllUsers";
import ChangeProjectOwner from "../../services/api/AuthorisationService/ProjectActions/ChangeProjectOwner";
import DeleteProject from "../../services/api/AuthorisationService/ProjectActions/DeleteProject";

const ProjectPage = () => {
  const { t } = useTranslation();
  const navigate = useNavigate();

  const [projects, setProjects] = useState([]);
  const [users, setUsers] = useState([]);
  const [show, setShow] = useState(false);
  const [selectedProjectList, setselectedProjectList] = useState([]);
  const [selectedUserIdList, setSelectedUserIdList] = useState([]);

  const [dataFetched, setDataFetched] = useState(false);

  const inputFilterValue = useRef();
  const [valueToChange, setValueToChange] = useState();

  const [deletePopupOpen, setDeletePopupOpen] = React.useState(false);
  const [ownerPopupOpen, setOwnerPopupOpen] = React.useState(false);

  const numberOfProject = projects.length;

  const handleDeletePopupClose = () => {
    setDeletePopupOpen(false);
  };

  const handleOwnerPopupClose = () => {
    setOwnerPopupOpen(false);
  };

  const handleFilter = () => {
    const enteredFilterValue = inputFilterValue.current.value;
    setValueToChange(enteredFilterValue);
  };

  useEffect(() => {
    const enteredFilterValue = valueToChange ? valueToChange : "";
    GetAllProjects({ enteredFilterValue })
      .then((response) => {
        var listObj = response.List.map((item, index) => {
          let tempObj = (typeof item === "string") ? JSON.parse(item) : item;
          tempObj.id = index + 1;
          return tempObj;
        });
        setProjects(listObj);
        setDataFetched(true);
        GetAllUsers()
          .then((response) => {
            listObj = response.users.map((item, index) => {
              let tempObj = (typeof item === "string") ? JSON.parse(item) : item;
              tempObj.id = index + 1;
              return tempObj;
            });
            setUsers(listObj);
          })
          .catch((response) => {
            console.log("GetAllUsers", response.description);
            navigate("/error");
          });
      })
      .catch((response) => {
        console.log("GetAllProjects", response.description);
        navigate("/error");
      });
  }, [valueToChange]);

  const handleDelete = () => {
    if (selectedProjectList.length === 0) {
      setDeletePopupOpen(true);
    }
    var index;

    for (index = 0; index < selectedProjectList.length; index++) {
      var projectNameToRemove = projects[selectedProjectList[index] - 1].name;

      DeleteProject({ projectNameToRemove }).then((response) => {
        handleFilter();
        console.log(response);
      });
    }
  };

  const selectProjectListEvent = (Idlist) => {
    setselectedProjectList([...Idlist]);
  };

  const modalToggle = () => {
    if (selectedProjectList.length === 0) {
      setOwnerPopupOpen(true);
      setShow(show);
    } else {
      setShow(!show);
    }
  };

  const ProjectOwnerHandler = (projectName, ownerUsername) => {
    ChangeProjectOwner({ projectName, ownerUsername })
      .then((response) => {
        console.log("ChangeProjectOwner", response);
      })
      .catch((error) => {
        console.log(error, "Change Project Owner Error");
      });
  };

  const onChaneOwnerBtnClick = () => {
    var projectList = selectedProjectList.map((item) => {
      var obj = projects.find((x) => x.id === item);
      if (obj === undefined) {
        return false;
      }
      return obj;
    });
    let selectedUsername = users.find((x) => x.id === selectedUserIdList[0]);
    projectList.forEach((item, index) => {
      ProjectOwnerHandler(item.name, selectedUsername.username);
    });
    const updatedProjectList = [...projects];
    selectedProjectList.forEach((element) => {
      let temp = updatedProjectList.find((x) => x.id === element);
      if (temp !== undefined) {
        temp.creatingUser = selectedUsername.username;
      }
    });
    setProjects(updatedProjectList);
    setShow(false);
  };

  const getSelectedUserId = (selectedUserId) => {
    setSelectedUserIdList([...selectedUserId]);
  };

  return (
    <div className="projectPage">
      <Sidebar
        items="project"
        onRemoved={handleDelete}
        openModal={modalToggle}
      />
      <div className="listContainer">
        <br />
        <br />
        <br />
        <Navbar />
        <InfoModal
          open={deletePopupOpen}
          handleClose={handleDeletePopupClose}
          dialogTitle={t("projectPage:deletePopupTitle")}
          dialogContent={t("projectPage:deletePopupContent")}
          dialogButton={t("projectPage:deletePopupButton")}
        />
        <InfoModal
          open={ownerPopupOpen}
          handleClose={handleOwnerPopupClose}
          dialogTitle={t("projectPage:ownerPopupTitle")}
          dialogContent={t("projectPage:ownerPopupContent")}
          dialogButton={t("projectPage:ownerPopupButton")}
        />
        <ChangeOwnerModal
          modal={show}
          modalTitle={t("projectPage:modalTitle")}
          handleToggle={modalToggle}
          userList={users}
          onUserIdSelect={getSelectedUserId}
          onSaveBtnClick={onChaneOwnerBtnClick}
        />
        {!dataFetched ? (
          <Spinner sx={{ marginLeft: "50%", marginTop: "30%" }} />
        ) : (
          <DataGrid
            list={projects}
            columnHeader={ProjectsColumnHeader}
            title="Projects"
            onItemSelect={selectProjectListEvent}
            input={inputFilterValue}
            handleFilter={handleFilter}
            numberOfProject={numberOfProject}
          />
        )}
      </div>
    </div>
  );
};

export default ProjectPage;
