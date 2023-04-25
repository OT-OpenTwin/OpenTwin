
import fesRequest from "./fesRequest";

export function msRequest(address, data) {

    return new Promise((resolve) => {
        fesRequest({
          method: "POST",
          mode: "no-cors",
          headers: { "content-type": "application/json" },
          data: JSON.stringify(data),
          url: address,
        })
          .then((res) => {
            return resolve(res);
          })
          .catch((err) => {
            resolve(err);
          });
      });
}

export default msRequest;
