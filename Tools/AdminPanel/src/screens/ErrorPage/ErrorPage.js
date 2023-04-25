import React from "react";
import { useTranslation } from "react-i18next";
import { Typography, Button } from "@mui/material";
import OTLogo from "../../resources/logo/logo.png";
import { Link } from "react-router-dom";
import "./Error.scss";

const ErrorPage = () => {
  const { t } = useTranslation();

  return (
    <div className="ErrorPage">
      <img
        src={OTLogo}
        width="60"
        height="60"
        alt="OT-Logo"
        className="rounded"
      />
      <Typography variant="h2" color="white">
        <br />
        <br />
        {t("errorPage:title")}
      </Typography>
      <div className="errorButton">
        <Typography variant="h6" color="white">
          {t("errorPage:text")}
        </Typography>
        <Button variant="outlined" color="primary">
          <Link to="/" style={{ textDecoration: "none" }}>
            <Typography variant="h7" color="white">
              {t("errorPage:button")}
            </Typography>
          </Link>
        </Button>
      </div>
    </div>
  );
};

export default ErrorPage;
