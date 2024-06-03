import "./sidebar.scss";
import React from "react";
import { Link } from "react-router-dom";
import { useTranslation } from "react-i18next";

import ManageAccountsIcon from "@mui/icons-material/ManageAccounts";
import PersonRemoveIcon from "@mui/icons-material/PersonRemove";
import LayersClearIcon from "@mui/icons-material/LayersClear";
import PersonAddAlt1Icon from "@mui/icons-material/PersonAddAlt1";
import AddCircleIcon from "@mui/icons-material/AddCircle";
import GroupRemoveIcon from "@mui/icons-material/GroupRemove";

export const Sidebar = (props) => {
  const { t } = useTranslation();

  const userPageListItems = (
    <div className="sidebar">
      <div className="top"></div>
      <div className="center">
        <br />
        <ul>
          <p className="title">{t("sidebar:userTitle")}</p>
          <Link to="/users/new" style={{ textDecoration: "none" }}>
            <li>
              <PersonAddAlt1Icon className="icon" />
              <span>{t("sidebar:newUser")}</span>
            </li>
          </Link>
        </ul>
        <ul>
          <li onClick={props.onRemoved}>
            <PersonRemoveIcon className="icon" />
            <span>{t("sidebar:deleteUser")}</span>
          </li>
        </ul>
      </div>
    </div>
  );

  const groupPageListItems = (
    <div className="sidebar">
      <div className="top"></div>
      <div className="center">
        <br />
        <ul>
          <p className="title">{t("sidebar:groupTitle")}</p>
          <Link to="/groups/new" style={{ textDecoration: "none" }}>
           </Link>
          <li onClick={props.openModal}>
            <ManageAccountsIcon className="icon" />
            <span> {t("sidebar:changeGroupOwner")}</span>
          </li>
          <li onClick={props.onRemoved}>
            <GroupRemoveIcon className="icon" />
            <span> {t("sidebar:deleteGroup")}</span>
          </li>
        </ul>
      </div>
    </div>
  );

  const projectPageListItems = (
    <div className="sidebar">
      <div className="top"></div>
      <div className="center">
        <br />
        <ul>
          <p className="title"> {t("sidebar:projectTitle")}</p>
          <li onClick={props.openModal}>
            <ManageAccountsIcon className="icon" />
            <span> {t("sidebar:changeProjectOwner")}</span>
          </li>
          <li onClick={props.onRemoved}>
            <LayersClearIcon className="icon" />
            <span>{t("sidebar:deleteProject")}</span>
          </li>
        </ul>
      </div>
    </div>
  );

  let items;
  if (props.items === "user") {
    items = userPageListItems;
  } else if (props.items === "group") {
    items = groupPageListItems;
  } else {
    items = projectPageListItems;
  }

  return (
    <div className="sidebar">
      <div className="top"></div>
      <div className="center">{items}</div>
    </div>
  );
};

export default Sidebar;
