import "./Card.scss";
import React from "react";

import {
  CardActionArea,
  Card,
  CardContent,
  CardMedia,
  Typography,
} from "@mui/material";

export default function CardComponent(props) {
  return (
    <>
      <div className="Card">
        <Card
          sx={{
            maxWidth: 300,
            marginLeft: "20%",
            marginTop: "20%",
            marginBottom: "20%",
            marginRight: "20%",
            backgroundColor: "transparent",
            color: "white",
            textAlign: "center",
          }}
        >
          <CardActionArea>
            <CardMedia component="img" image={props.icon} alt=" " />
            <CardContent>
              <Typography gutterBottom variant="button">
                {props.title}
              </Typography>
              <Typography variant="h3" color="white">
                {props.number}
              </Typography>
            </CardContent>
          </CardActionArea>
        </Card>
      </div>
    </>
  );
}
