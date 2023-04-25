
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

// Imports
extern crate core;
extern crate env_logger;
extern crate log;
extern crate libc;
extern crate url;

use libloading::Symbol; // Loading of dynamic external libraries.
use libc::c_char; // C char type to be used with external c++ libraries

use std::env; // Used to get the program args

use warp::Filter; // Warp filter/endpoint
use std::ffi::{ CStr, CString };
use std::net::SocketAddr;
use tokio::net;
use url::Url;

use open_twin::handler;
use crate::serve_mtls::serve_mtls_connection; // Our handler..


mod serve_mtls;
mod mtls_config;

// creating a new type of type function with the following args which -> returns int32 (i32)
type InitAPI = fn(site_id: *const c_char, service_url: *const c_char, database_url: *const c_char, directory_url: *const c_char) -> i32;

// Same logic here also
type MicroserviceRequestAPI = fn() -> *const c_char;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>>
{

    env_logger::init();

    // Getting the program arguments
    let args: Vec<String> = env::args().collect();

    let lib_path = &args[1];

	let mut service_url = "";
	
	let mut site_id = "";
	let mut db_url  = "";
	let mut dir_url = "";

    if args.len() > 2
    {
		site_id = &args[2];
    }
	
	if args.len() > 3
    {
        service_url = &args[3];
    }
	
	if args.len() > 4
    {
        db_url = &args[4];
    }

	if args.len() > 5
    {
        dir_url = &args[5];
    }

    // Setting the Library path
    let lib = handler::set_lib_path(lib_path);
		
    unsafe
    {
        let initialize: Symbol<InitAPI> = lib.get(b"init").unwrap(); // getting the function with name init

        // Turning the strings into C strings to use them with our fetched function above (init)
		let siteid_c_str = CString::new(site_id).unwrap();
		let service_c_str = CString::new(service_url).unwrap();
		let db_c_str = CString::new(db_url).unwrap();
		let dir_c_str = CString::new(dir_url).unwrap();

        // Calling the function and getting its result
        let _result = initialize(siteid_c_str.as_ptr(), service_c_str.as_ptr(), db_c_str.as_ptr(), dir_c_str.as_ptr());
		if _result != 0 {
			eprintln!("Library/Service initialization ({}:init()) failed with exit code {}", lib_path, _result);
            std::process::exit(_result);
		}

		// Now we get the actual URL of the service (it might be overridden in debug mode by a config file)
		let get_service_url: Symbol<MicroserviceRequestAPI> = lib.get(b"getServiceURL").unwrap();
	    let c_buf = get_service_url();

        // from pointer to string
        service_url = CStr::from_ptr(c_buf).to_str().unwrap();
    }

    // GET /any path

    let index = warp::path::end().map(|| "Endpoint, /execute /queue /shutdown");

    let execute_route = warp::path("execute").and(
        warp::post()
            .and(warp::ext::get::<rustls::Certificate>())
            .and(warp::ext::get::<SocketAddr>())
            .and(warp::body::json())
            .and_then(handler::execute),
    );


    let execute_one_tls_route = warp::path("execute-one-way-tls").and(
        warp::post()
            .and(warp::ext::get::<SocketAddr>())
            .and(warp::body::json())
            .and_then(handler::execute_one_way_tls),
    );



    let queue_route = warp::path("queue").and(
        warp::post()
            .and(warp::ext::get::<rustls::Certificate>())
            .and(warp::ext::get::<SocketAddr>())
            .and(warp::body::json())
            .and_then(handler::queue),
    );


    // Should the shutdown route also be protected?
    // If so, the certificate has to be added
	let shutdown_route = warp::path("shutdown").and(
        warp::post()
            .and(warp::ext::get::<rustls::Certificate>())
            .and(warp::body::json())
            .and_then(handler::shutdown),
    );

    let routes = index.or(execute_route).or(execute_one_tls_route).or(queue_route).or(shutdown_route);





    // MTLS Part:

    // We want to publish a different ip to other services (usually this is the public ip), than the ip we listen on.
    // This is especially handy on cases where OpenTwin runs inside a container with isolated network.
    let service_port = Url::parse(&format!("https://{}", service_url))
        .expect(&format!("Invalid service url. Unable to parse service url: {}", service_url))
        .port();
    if service_port.is_none()
    {
        panic!("Invalid service url. Service url is lacking port defintion: {}", service_url);
    }
    let binding_address = format!("0.0.0.0:{}", service_port.unwrap().to_string());
    let listener = net::TcpListener::bind(&binding_address).await?;

    println!("Server listening on {:?} (publishing {:?})", binding_address, service_url);




    // Serve the routes and wait for TCP connections which are protected by mTLS
    loop
    {
		let cors = warp::cors()
			.allow_any_origin()
			.allow_headers(vec!["User-Agent", "Sec-Fetch-Mode", "Referer", "Origin", "Access-Control-Request-Method", "Access-Control-Request-Headers", "content-type"])
			.allow_methods(vec!["POST", "GET"]);
		
        if let Err(e) = serve_mtls_connection(&listener, routes.with(cors)).await
        {
            eprintln!("Problem accepting TLS connection: {}", e);
        }
    }

}
