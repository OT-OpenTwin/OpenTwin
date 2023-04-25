import React from "react";
import Box from "@mui/material/Box";
import Typography from "@mui/material/Typography";
import Container from "@mui/material/Container";
import "./footer.scss";

function Copyright() {
  return (
    <Typography variant="body" color="white">
      {"Copyright © "}
      OPENTWIN {new Date().getFullYear()}
    </Typography>
  );
}

export default function StickyFooter() {
  return (
    <div className="footer">
      <Box
        component="footer"
        sx={{
          py: 1,
          px: 1,
          mt: "auto",
        }}
      >
        <Container maxWidth="sm">
          <Typography variant="body1" />
          <Copyright />
        </Container>
      </Box>
    </div>
  );
}
