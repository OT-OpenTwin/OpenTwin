/* eslint-disable react-hooks/exhaustive-deps */
import "./style/UserPage.scss";
import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { useTranslation } from "react-i18next";

import Sidebar from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import InfoModal from "../../components/Modal/InfoModal/InfoModal";
import DataGrid from "../../components/DataGrid/DataGrid";
import { CircularProgress as Spinner } from "@mui/material";

import { UserColumnHeader } from "../../resources/dataGridSources";
import GetAllUsers from "../../services/api/AuthorisationService/UserActions/GetAllUsers";
import DeleteUser from "../../services/api/AuthorisationService/UserActions/DeleteUser";

const UserPage = () => {
  const { t } = useTranslation();

  const navigate = useNavigate();

  const [users, setUsers] = useState([]);
  const [selectedUserList, setselectedUserList] = useState([]);
  const [dataFetched, setDataFetched] = useState(false);

  const [infoPopupOpen, setInfoPopupOpen] = React.useState(false);

  const handleClose = () => {
    setInfoPopupOpen(false);
  };

  useEffect(() => {
    GetAllUsers()
      .then((response) => {
        var listObj = response.users.map((item, index) => {
          let tempObj = JSON.parse(item);
          tempObj.id = index + 1;
          return tempObj;
        });
        setUsers(listObj);
        setDataFetched(true);
      })
      .catch((response) => {
        console.log("GetAllUsers", response.description);
        navigate("/error");
      });
  }, []);

  const selectUserEvent = (Idlist) => {
    setselectedUserList([...Idlist]);
  };

  const handleDelete = () => {
    if (selectedUserList.length === 0) {
      setInfoPopupOpen(true);
    }
    var i;
    let users2 = [...users];

    for (i = 0; i < selectedUserList.length; i++) {
      var usernameToDelete = users[selectedUserList[i] - 1].username;
      var j;
      let index = -1
      for (j=0; j< users2.length; j++) {
        if (users2[j].username ===  usernameToDelete) 
        {
          index = j;
        }
      }
      if (index !== -1) {
        users2.splice(index, 1);
      }

      DeleteUser({ usernameToDelete }).then((response) => {
        console.log(response);
      });
    }
    setUsers(users2);
  };

  return (
    <div className="userPage">
      <Sidebar items="user" onRemoved={handleDelete} />
      <div className="listContainer">
        <Navbar />
        <InfoModal
          open={infoPopupOpen}
          handleClose={handleClose}
          dialogTitle={t("userPage:dialogTitle")}
          dialogContent={t("userPage:dialogContent")}
          dialogButton={t("userPage:dialogButton")}
        />
        <br />
        <br />
        <br />
        {!dataFetched ? (
          <Spinner sx={{ marginLeft: "50%", marginTop: "30%" }} />
        ) : (
          <DataGrid
            list={users}
            columnHeader={UserColumnHeader}
            title="Users"
            onItemSelect={selectUserEvent}
          />
        )}
      </div>
    </div>
  );
};

export default UserPage;
