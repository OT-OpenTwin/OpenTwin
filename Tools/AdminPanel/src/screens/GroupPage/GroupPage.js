/* eslint-disable react-hooks/exhaustive-deps */
import "./style/GroupPage.scss";
import React, { useState, useEffect } from "react";
import { useTranslation } from "react-i18next";
import { useNavigate } from "react-router-dom";

import { Sidebar } from "../../components/Sidebar/Sidebar";
import Navbar from "../../components/Navbar/Navbar";
import DataGrid from "../../components/DataGrid/DataGrid";
import ChangeOwnerModal from "../../components/Modal/ChangeOwnerModal/ChangeOwnerModal";
import InfoModal from "../../components/Modal/InfoModal/InfoModal";
import { CircularProgress as Spinner } from "@mui/material";

import { GroupsColumnHeader } from "../../resources/dataGridSources";
import GetAllGroups from "../../services/api/AuthorisationService/GroupActions/GetAllGroups";
import GetAllUsers from "../../services/api/AuthorisationService/UserActions/GetAllUsers";
import ChangeGroupOwner from "../../services/api/AuthorisationService/GroupActions/ChangeGroupOwner";
import DeleteGroup from "../../services/api/AuthorisationService/GroupActions/DeleteGroup";

const GroupPage = () => {
  const { t } = useTranslation();
  const navigate = useNavigate();

  const [groups, setGroups] = useState([]);
  const [users, setUsers] = useState([]);
  const [show, setShow] = useState(false);
  const [selectedGroupList, setSelectedGroupList] = useState([]);
  const [selectedUserdIdList, setSelectedUserdIdList] = useState([]);

  const [dataFetched, setDataFetched] = useState(false);

  const [deletePopupOpen, setDeletePopupOpen] = React.useState(false);
  const [ownerPopupOpen, setOwnerPopupOpen] = React.useState(false);

  const handleDeletePopupClose = () => {
    setDeletePopupOpen(false);
  };

  const handleOwnerPopupClose = () => {
    setOwnerPopupOpen(false);
  };

  useEffect(() => {
    GetAllGroups()
      .then((response) => {
        var listObj = response.groups.map((item, index) => {
          let tempObj = JSON.parse(item);
          tempObj.id = index + 1;
          return tempObj;
        });
        setGroups(listObj);
        setDataFetched(true);
        GetAllUsers()
          .then((response) => {
            listObj = response.users.map((item, index) => {
              let tempObj = JSON.parse(item);
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
        console.log("GetAllGroups", response.description);
        navigate("/error");
      });
  }, []);

  const handleDelete = () => {
    if (selectedGroupList.length === 0) {
      setDeletePopupOpen(true);
    }
    var i;
    let groups2 = [...groups];

    for (i = 0; i < selectedGroupList.length; i++) {
      var groupNameToRemove = groups[selectedGroupList[i] - 1].groupName;
      var j;
      let index = -1
      for (j=0; j< groups2.length; j++) {
        if (groups2[j].groupName ===  groupNameToRemove) 
        {
          index = j;
        }
      }
      if (index !== -1) {
        groups2.splice(index, 1);
      }

      console.log("GetAllUsers", groupNameToRemove);
      DeleteGroup({ groupNameToRemove }).then((response) => {
        console.log(response);
      });
    }
    setGroups(groups2);
  };

  const selectGroupListEvent = (Idlist) => {
    setSelectedGroupList([...Idlist]);
  };

  const modalToggle = () => {
    if (selectedGroupList.length === 0) {
      setOwnerPopupOpen(true);
      setShow(show);
    } else {
      setShow(!show);
    }
  };

  const GroupOwnerHanlder = (grpName, ownerUsername) => {
    ChangeGroupOwner({ grpName, ownerUsername }).then((response) => {
      console.log("ChangeGroupOwner", response);
    });
  };

  const onChaneOwnerBtnClick = () => {
    var grpList = selectedGroupList.map((item) => {
      var obj = groups.find((x) => x.id === item);
      if (obj === undefined) {
        return false;
      }
      return obj;
    });
    console.log("grpList", grpList);
    let selectedUsername = users.find((x) => x.id === selectedUserdIdList[0]);
    grpList.forEach((item, index) => {
      console.log("itemGr", item);
      GroupOwnerHanlder(item.groupName, selectedUsername.username);
    });
    const updatedGroupList = [...groups];
    selectedGroupList.forEach((element) => {
      let temp = updatedGroupList.find((x) => x.id === element);
      if (temp !== undefined) {
        temp.ownerUsername = selectedUsername.username;
      }
    });
    setGroups(updatedGroupList);
    setShow(false);
  };

  const getSelectedUserId = (selectedUserId) => {
    setSelectedUserdIdList([...selectedUserId]);
  };

  return (
    <div className="groupPage">
      <Sidebar items="group" onRemoved={handleDelete} openModal={modalToggle} />
      <div className="listContainer">
        <br />
        <br />
        <br />
        <Navbar />
        <InfoModal
          open={deletePopupOpen}
          handleClose={handleDeletePopupClose}
          dialogTitle={t("groupPage:deletePopupTitle")}
          dialogContent={t("groupPage:deletePopupContent")}
          dialogButton={t("groupPage:deletePopupButton")}
        />
        <InfoModal
          open={ownerPopupOpen}
          handleClose={handleOwnerPopupClose}
          dialogTitle={t("groupPage:ownerPopupTitle")}
          dialogContent={t("groupPage:ownerPopupContent")}
          dialogButton={t("groupPage:ownerPopupButton")}
        />
        <ChangeOwnerModal
          modal={show}
          modalTitle={t("groupPage:modalTitle")}
          handleToggle={modalToggle}
          userList={users}
          onUserIdSelect={getSelectedUserId}
          onSaveBtnClick={onChaneOwnerBtnClick}
        />
        {!dataFetched ? (
          <Spinner sx={{ marginLeft: "50%", marginTop: "30%" }} />
        ) : (
          <DataGrid
            list={groups}
            columnHeader={GroupsColumnHeader}
            title="Groups"
            onItemSelect={selectGroupListEvent}
          />
        )}
      </div>
    </div>
  );
};

export default GroupPage;
