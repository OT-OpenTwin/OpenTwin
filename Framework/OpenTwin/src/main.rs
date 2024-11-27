
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
use std::path::Path;

use warp::Filter; // Warp filter/endpoint
use std::ffi::{ CStr, CString };
use std::net::SocketAddr;
use tokio::net;
use url::Url;
use warp::{http::StatusCode, reply};

use open_twin::handler;
use crate::serve_mtls::serve_mtls_connection; // Our handler..

static GLOBAL: state::Storage<String> = state::Storage::new();

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

    GLOBAL.set(lib_path.to_string());
	
    // creating path to file which will be downloaded depending on if it is an enduser or a developer machine
    let download_file_path: String = if option_env!("OPENTWIN_DEV_ROOT").is_some() {
        env!("OPENTWIN_DEV_ROOT").to_owned() + "/Deployment/FrontendInstaller/Install_OpenTwin_Frontend.exe"
    } else {
        std::env::current_exe().unwrap().to_str().unwrap().to_string() + "/FrontendInstaller/Install_OpenTwin_Frontend.exe"
    };
    let dll_file_name = Path::new(GLOBAL.get()).file_name().unwrap().to_str();
    let service_name = Path::new(dll_file_name.unwrap()).file_stem().unwrap().to_str();

    // http endpoint for file download
    if service_name == Some("GlobalSessionService") {
        let download_route = warp::path::end()
            .map(|| warp::reply::html(get_download_html_body()));

        let installer_route = warp::path("installer")
            .and(warp::fs::file(download_file_path));
        
        tokio::task::spawn(async move {
            warp::serve(download_route.or(installer_route))
            .run(([0, 0, 0, 0], 80))
            .await;
        });

        println!("HTTP Server listening on \"0.0.0.0:80\"")
    }
    
    // GET /any path
    let info_route = warp::path::end()
        .map(move || {
            let computed_string: String = "OpenTwin Microservice (".to_string() + service_name.expect("UNKNOWN") + ")";
            return reply::with_status(
                computed_string,
                StatusCode::CREATED,
            );
	    });

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

    let routes = info_route.or(execute_route).or(execute_one_tls_route).or(queue_route).or(shutdown_route);

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

    println!("HTTPS Server listening on {:?} (publishing {:?})", binding_address, service_url);

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

fn get_download_html_body() -> String {
    "   <!DOCTYPE html>
        <head>
            <title>OpenTwin</title>
        </head>
        <body>
            <img alt=\"Could not load OpenTwin logo\" src=\"data:image/png; base64, 
            iVBORw0KGgoAAAANSUhEUgAAAfQAAAH0CAYAAADL1t+KAAAACXBIWXMAACZzAAAmcwHzbHUKAAAL
            hWlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0w
            TXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRh
            LyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgOS4wLWMwMDAgNzkuMTcxYzI3ZiwgMjAyMi8wOC8x
            Ni0xODowMjo0MyAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9y
            Zy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9
            IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIiB4bWxuczpwaG90b3No
            b3A9Imh0dHA6Ly9ucy5hZG9iZS5jb20vcGhvdG9zaG9wLzEuMC8iIHhtbG5zOmRjPSJodHRwOi8v
            cHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5j
            b20veGFwLzEuMC9tbS8iIHhtbG5zOnN0RXZ0PSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAv
            c1R5cGUvUmVzb3VyY2VFdmVudCMiIHhtbG5zOnN0UmVmPSJodHRwOi8vbnMuYWRvYmUuY29tL3hh
            cC8xLjAvc1R5cGUvUmVzb3VyY2VSZWYjIiB4bWxuczp0aWZmPSJodHRwOi8vbnMuYWRvYmUuY29t
            L3RpZmYvMS4wLyIgeG1sbnM6ZXhpZj0iaHR0cDovL25zLmFkb2JlLmNvbS9leGlmLzEuMC8iIHht
            cDpDcmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIDI0LjAgKFdpbmRvd3MpIiB4bXA6Q3JlYXRl
            RGF0ZT0iMjAyMi0xMS0xNFQyMzoxMzo0NiswMTowMCIgeG1wOk1ldGFkYXRhRGF0ZT0iMjAyMi0x
            Mi0xNlQyMzowNzo1OSswMTowMCIgeG1wOk1vZGlmeURhdGU9IjIwMjItMTItMTZUMjM6MDc6NTkr
            MDE6MDAiIHBob3Rvc2hvcDpDb2xvck1vZGU9IjMiIGRjOmZvcm1hdD0iaW1hZ2UvcG5nIiB4bXBN
            TTpJbnN0YW5jZUlEPSJ4bXAuaWlkOjdhYWU3ODE1LTNkOTktZWQ0ZC04MGFhLThlMmMzZTE4Yzg4
            MCIgeG1wTU06RG9jdW1lbnRJRD0iYWRvYmU6ZG9jaWQ6cGhvdG9zaG9wOjIyOGY2Zjk4LTkzMTIt
            Njk0NS05MTlmLTdkYjNhNjFhNTFiOCIgeG1wTU06T3JpZ2luYWxEb2N1bWVudElEPSJ4bXAuZGlk
            OjJlODYxMzI2LThhODMtZDY0OS04Y2E0LWM4MmE3NzY0MGQ4NyIgdGlmZjpPcmllbnRhdGlvbj0i
            MSIgdGlmZjpYUmVzb2x1dGlvbj0iMjUwMDAwMC8xMDAwMCIgdGlmZjpZUmVzb2x1dGlvbj0iMjUw
            MDAwMC8xMDAwMCIgdGlmZjpSZXNvbHV0aW9uVW5pdD0iMiIgZXhpZjpDb2xvclNwYWNlPSI2NTUz
            NSIgZXhpZjpQaXhlbFhEaW1lbnNpb249IjUwMCIgZXhpZjpQaXhlbFlEaW1lbnNpb249IjUwMCI+
            IDxwaG90b3Nob3A6VGV4dExheWVycz4gPHJkZjpCYWc+IDxyZGY6bGkgcGhvdG9zaG9wOkxheWVy
            TmFtZT0iTyIgcGhvdG9zaG9wOkxheWVyVGV4dD0iTyIvPiA8cmRmOmxpIHBob3Rvc2hvcDpMYXll
            ck5hbWU9IlQiIHBob3Rvc2hvcDpMYXllclRleHQ9IlQiLz4gPHJkZjpsaSBwaG90b3Nob3A6TGF5
            ZXJOYW1lPSItIiBwaG90b3Nob3A6TGF5ZXJUZXh0PSItIi8+IDxyZGY6bGkgcGhvdG9zaG9wOkxh
            eWVyTmFtZT0iT1BFTiBUV0lOIiBwaG90b3Nob3A6TGF5ZXJUZXh0PSJPUEVOIFRXSU4iLz4gPHJk
            ZjpsaSBwaG90b3Nob3A6TGF5ZXJOYW1lPSJPIiBwaG90b3Nob3A6TGF5ZXJUZXh0PSJPIi8+IDxy
            ZGY6bGkgcGhvdG9zaG9wOkxheWVyTmFtZT0iVCIgcGhvdG9zaG9wOkxheWVyVGV4dD0iVCIvPiA8
            cmRmOmxpIHBob3Rvc2hvcDpMYXllck5hbWU9Ii0iIHBob3Rvc2hvcDpMYXllclRleHQ9Ii0iLz4g
            PC9yZGY6QmFnPiA8L3Bob3Rvc2hvcDpUZXh0TGF5ZXJzPiA8eG1wTU06SGlzdG9yeT4gPHJkZjpT
            ZXE+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJjcmVhdGVkIiBzdEV2dDppbnN0YW5jZUlEPSJ4bXAu
            aWlkOjJlODYxMzI2LThhODMtZDY0OS04Y2E0LWM4MmE3NzY0MGQ4NyIgc3RFdnQ6d2hlbj0iMjAy
            Mi0xMS0xNFQyMzoxMzo0NiswMTowMCIgc3RFdnQ6c29mdHdhcmVBZ2VudD0iQWRvYmUgUGhvdG9z
            aG9wIDI0LjAgKFdpbmRvd3MpIi8+IDxyZGY6bGkgc3RFdnQ6YWN0aW9uPSJzYXZlZCIgc3RFdnQ6
            aW5zdGFuY2VJRD0ieG1wLmlpZDoyZDdjM2MyNy0yMzA4LTQ5NDMtOTEzZC01OGI3N2YwMTRlZjAi
            IHN0RXZ0OndoZW49IjIwMjItMTItMTZUMjM6MDc6NTkrMDE6MDAiIHN0RXZ0OnNvZnR3YXJlQWdl
            bnQ9IkFkb2JlIFBob3Rvc2hvcCAyNC4xIChXaW5kb3dzKSIgc3RFdnQ6Y2hhbmdlZD0iLyIvPiA8
            cmRmOmxpIHN0RXZ0OmFjdGlvbj0iY29udmVydGVkIiBzdEV2dDpwYXJhbWV0ZXJzPSJmcm9tIGFw
            cGxpY2F0aW9uL3ZuZC5hZG9iZS5waG90b3Nob3AgdG8gaW1hZ2UvcG5nIi8+IDxyZGY6bGkgc3RF
            dnQ6YWN0aW9uPSJkZXJpdmVkIiBzdEV2dDpwYXJhbWV0ZXJzPSJjb252ZXJ0ZWQgZnJvbSBhcHBs
            aWNhdGlvbi92bmQuYWRvYmUucGhvdG9zaG9wIHRvIGltYWdlL3BuZyIvPiA8cmRmOmxpIHN0RXZ0
            OmFjdGlvbj0ic2F2ZWQiIHN0RXZ0Omluc3RhbmNlSUQ9InhtcC5paWQ6N2FhZTc4MTUtM2Q5OS1l
            ZDRkLTgwYWEtOGUyYzNlMThjODgwIiBzdEV2dDp3aGVuPSIyMDIyLTEyLTE2VDIzOjA3OjU5KzAx
            OjAwIiBzdEV2dDpzb2Z0d2FyZUFnZW50PSJBZG9iZSBQaG90b3Nob3AgMjQuMSAoV2luZG93cyki
            IHN0RXZ0OmNoYW5nZWQ9Ii8iLz4gPC9yZGY6U2VxPiA8L3htcE1NOkhpc3Rvcnk+IDx4bXBNTTpE
            ZXJpdmVkRnJvbSBzdFJlZjppbnN0YW5jZUlEPSJ4bXAuaWlkOjJkN2MzYzI3LTIzMDgtNDk0My05
            MTNkLTU4Yjc3ZjAxNGVmMCIgc3RSZWY6ZG9jdW1lbnRJRD0ieG1wLmRpZDoyZTg2MTMyNi04YTgz
            LWQ2NDktOGNhNC1jODJhNzc2NDBkODciIHN0UmVmOm9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRp
            ZDoyZTg2MTMyNi04YTgzLWQ2NDktOGNhNC1jODJhNzc2NDBkODciLz4gPC9yZGY6RGVzY3JpcHRp
            b24+IDwvcmRmOlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIiPz56dhwRAAAmkElE
            QVR4nO3debhlVX3m8e9BBgUFqlRAjIiFCiIyCCLOSqioUeOQgMaBOOSBqImd1o5Vxqkd0ilMaNsY
            OwGj7RCjgkaM2NpSGo1TRBAVEVQoBxSVqQCZp91/rFNFVd1bVWfvs8/5rb3W9/M89wEU7nnvrbPP
            u9fae681apoGSZI0bNtEB5AkSdOz0CVJKoCFLklSASx0SZIKYKFLklQAC12SpAJY6JIkFcBClySp
            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
            KoCFLklSAf4/G8ayBHfTCwQAAAAASUVORK5CYII=\">

            <div id=\"welcome_txt\">Welcome to OpenTwin</div>
            <a href=\"http://127.0.0.1:80/installer/Install_OpenTwin_Frontend.exe\" download>
                <button id=\"downloadBtn\">Download</button>
            </a>

            <style>
                body {
                    background-color: #04254E;
                    font-family: 'Courier New', Courier, monospace;
                }

                img {
                    left: 50%;
                    top: 35%;
                    width: 200px;
                    height: 200px;
                    position: absolute;
                    transform: translate(-50%, -50%);
                }

                #welcome_txt {
                    top: 50%;
                    left: 50%;
                    position: absolute;
                    transform: translate(-50%, -50%);
                    font-size: 20px;
                    color: white;
                }
        
                #downloadBtn {
                    width: 150px;
                    height: 50px;
                    top: 60%;
                    left: 50%;
                    border: none;
                    border-radius: 40px;
                    position: absolute;
                    cursor: pointer;
                    transform: translate(-50%, -60%);
                    background-color: #7ca0c4;
                    color: white;
                    font-size: large;
                    font-family: 'Courier New', Courier, monospace;
                }

                #downloadBtn:hover {
                    background-color: #2f5c92;
                }
            </style>
            <script type=\"text/javascript\">
                document.getElementById(\"downloadBtn\").addEventListener(\"click\", () => {
                    alert(\"Downloading Installer...\\nPlease execute the installer after the download.\");
                });
            </script>
        </body>".to_string()
}