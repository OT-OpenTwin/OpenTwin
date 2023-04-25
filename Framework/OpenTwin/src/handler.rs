extern crate libloading;
extern crate serde_json;
extern crate state;

// use super::model::{PerformAction, QueueAction};
use libloading::Library;
use std::convert::Infallible;
use std::process;
use std::net::SocketAddr;
// use warp::{reply, Reply};
use serde_json::{Value};
use x509_parser::certificate;
use x509_parser::traits::FromDer;

static GLOBAL: state::Storage<libloading::Library> = state::Storage::new();

#[path = "service.rs"]
mod service;

/**
* Async function that gets a Json value and the Socket Address.
* It will call Service to perform the heavy load
*/
pub async fn execute(cert: rustls::Certificate, addr: SocketAddr, opts: serde_json::Value) -> Result<String, Infallible>
{
    if let Ok((_, _parsed_cert)) = certificate::X509Certificate::from_der(&cert.0)
    {
        // println!("Got a client certificate with subject: {}", parsed_cert.subject())
    } else
    {
        panic!("Got something in the certificate extension but cannot parse it\n")
    }

    let json_value = serde_json::to_string(&opts).unwrap();
	let addr_value = format!("{:?}", addr);
    let data = service::execute(json_value, addr_value).await.unwrap_or_else(|error| {
        panic!("Invalid Api call: {:?}", error);
    });

    Ok(data)
}

pub async fn execute_one_way_tls(addr: SocketAddr, opts: serde_json::Value) -> Result<String, Infallible>
{

    let json_value = serde_json::to_string(&opts).unwrap();
    let addr_value = format!("{:?}", addr);
    let data = service::execute_one_way_tls(json_value, addr_value).await.unwrap_or_else(|error| {
        panic!("Invalid Api call: {:?}", error);
    });

    Ok(data)
}

pub async fn queue(cert: rustls::Certificate, addr: SocketAddr, opts: Value) -> Result<String, Infallible>
{
    if let Ok((_, _parsed_cert)) = certificate::X509Certificate::from_der(&cert.0)
    {
        // println!("Got a client certificate with subject: {}", parsed_cert.subject())
    } else
    {
        panic!("Got something in the certificate extension but cannot parse it\n")
    }

    let json_value = serde_json::to_string(&opts).unwrap();
	let addr_value = format!("{:?}", addr);
    let data = service::queue(json_value, addr_value).await.unwrap_or_else(|error| {
        panic!("Invalid Api call: {:?}", error);
    });

    Ok(data)
}


pub async fn shutdown(cert: rustls::Certificate, _opts: Value) -> Result<String, Infallible>
{
    if let Ok((_, _parsed_cert)) = certificate::X509Certificate::from_der(&cert.0)
    {
        // println!("Got a client certificate with subject: {}", parsed_cert.subject())
    } else
    {
        panic!("Got something in the certificate extension but cannot parse it\n")
    }

	process::exit(0);
//	let data = "shutdown";
//	Ok(data.to_string())
}

/**
* Simple function that sets the Library path globally.
* Returns the mentioned Library variable.
*/
pub fn set_lib_path(lib_path: &String) -> &Library
{
    let lib = Library::new(lib_path).unwrap();
    GLOBAL.set(lib);
    return &*GLOBAL.get();
}
