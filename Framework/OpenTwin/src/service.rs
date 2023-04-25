extern crate libc;
extern crate libloading;

use super::GLOBAL;
use libc::c_char;
use libloading::Symbol;
use std::ffi::{CStr, CString};

use std::convert::Infallible;

// Creating the Function types that are relevant
type MicroserviceInputAPI = fn(*const c_char, *const c_char) -> *const c_char;
type MicroserviceReturnAPI = fn(*const c_char) -> *const c_char;

pub async fn execute(json_value: String, addr_value: String) -> Result<String, Infallible>
{
    // Dll library path from the global container
    let lib = &*GLOBAL.get();

    // Turning our normal strings into C Strings so that we can use them with our libraries
    // create c string from json value and address
    let model_json = CString::new(json_value).expect("CString::new failed");
	let model_addr = CString::new(addr_value).expect("CString::new failed");

    unsafe
    {
        let perform_action: Symbol<MicroserviceInputAPI> = lib.get(b"performAction").unwrap();

        // convert strings to pointer to pass to the function
        let c_buf = perform_action(model_json.as_ptr(), model_addr.as_ptr());

        // from pointer to string
        let c_str = CStr::from_ptr(c_buf).to_str().unwrap();

		// format data
        let data = format!("{}", c_str);
        
		// deallocate data string to free memory
        let deallocate_data: Symbol<MicroserviceReturnAPI> = lib.get(b"deallocateData").unwrap();
        deallocate_data(c_buf);

        Ok(data)
    }
}

pub async fn execute_one_way_tls(json_value: String, addr_value: String) -> Result<String, Infallible>
{
    // Dll library path from the global container
    let lib = &*GLOBAL.get();

    // Turning our normal strings into C Strings so that we can use them with our libraries
    // create c string from json value and address
    let model_json = CString::new(json_value).expect("CString::new failed");
    let model_addr = CString::new(addr_value).expect("CString::new failed");

    unsafe
        {
            let perform_action: Symbol<MicroserviceInputAPI> = lib.get(b"performActionOneWayTLS").unwrap();

            // convert strings to pointer to pass to the function
            let c_buf = perform_action(model_json.as_ptr(), model_addr.as_ptr());

            // from pointer to string
            let c_str = CStr::from_ptr(c_buf).to_str().unwrap();

            // format data
            let data = format!("{}", c_str);

            // deallocate data string to free memory
            let deallocate_data: Symbol<MicroserviceReturnAPI> = lib.get(b"deallocateData").unwrap();
            deallocate_data(c_buf);

            Ok(data)
        }
}

pub async fn queue(json_value: String, addr_value: String) -> Result<String, Infallible> {
    // Dll library path from the global
    let lib = &*GLOBAL.get();

    // create c string from json value
    let model_json = CString::new(json_value).expect("CString::new failed");
	let model_addr = CString::new(addr_value).expect("CString::new failed");

    unsafe {
        let queue_action: Symbol<MicroserviceInputAPI> = lib.get(b"queueAction").unwrap();

        // convert json string to pointer to pass to the function
        let c_buf = queue_action(model_json.as_ptr(), model_addr.as_ptr());

        // from pointer to string
        // let c_str = CStr::from_ptr(c_buf).to_str().unwrap();

        // deallocate data string to free memory
        let deallocate_data: Symbol<MicroserviceReturnAPI> = lib.get(b"deallocateData").unwrap();
        deallocate_data(c_buf);

        // format data
        let data = format!("queue action called");
        Ok(data)
    }
}
