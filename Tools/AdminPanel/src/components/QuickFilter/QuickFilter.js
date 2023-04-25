import "./QuickFilter.scss";
import React from "react";
import { TextField } from "@mui/material";
import FilterListIcon from "@mui/icons-material/FilterList";

const QuickFilter = (props) => {
  return (
    <div className="filter">
      <FilterListIcon sx={{ color: "action.active", mr: 1, my: 2 }} />
      <TextField
        label="Filter"
        size="small"
        variant="standard"
        type="text"
        inputRef={props.input}
        onChange={props.handleFilter}
      />
    </div>
  );
};

export default QuickFilter;
