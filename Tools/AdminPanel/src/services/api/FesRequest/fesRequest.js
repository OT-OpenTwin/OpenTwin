/* eslint-disable no-unused-vars */
import axios from "axios";

const fesRequest = (props) => {
  const { method, url, data, mode, headers } = props;

  return new Promise((resolve, reject) => {
    axios({ method, mode, headers, data, url }).then(
      (response) => {
        const result =
          response?.status === 200 ? response.data : response;
        resolve(result);
      },
      (error) => {
        reject(error);
      }
    );
  });
};

export default fesRequest;
