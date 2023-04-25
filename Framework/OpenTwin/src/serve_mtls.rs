use tokio::net;
use hyper::{service, service::Service};
use rustls::{Session};
use std::{env, io};
use futures::TryFutureExt;
use crate::mtls_config;

pub async fn serve_mtls_connection<F>(
    listener: &net::TcpListener,
    warp_filter: F,
) -> io::Result<()>
    where
        F: warp::Filter + Clone + Send + Sync + 'static,
        <F::Future as futures::TryFuture>::Ok: warp::Reply,
{

    // Wait for an incoming TCP connection
    let (socket, client_addr) = listener.accept().await?;
    println!("Received connection from {}", client_addr);


    // Hand off actual request handling to a new tokio task
    tokio::task::spawn(async move
    {
        let ca_cert_path = env::var("OPEN_TWIN_CA_CERT").unwrap();
        let server_cert_path = env::var("OPEN_TWIN_SERVER_CERT").unwrap();
        let server_key_path = env::var("OPEN_TWIN_SERVER_CERT_KEY").unwrap();

        let tls_acceptor = mtls_config::get_mtls_acceptor(
            &server_cert_path,
            &server_key_path,
            &ca_cert_path
        ).await;

        // Interpret data coming through the TCP stream as a TLS stream
        let stream = tls_acceptor
            .accept(socket)
            .map_err(|err|
                {
                    io::Error::new(
                        io::ErrorKind::Other,
                        format!("Problem accepting TLS connection: {:?}", err),
                    )
                })
            .await
            .unwrap();

        // Pull the client certificate out of the TLS session
        let (_, session) = stream.get_ref();

        let client_cert = session.get_peer_certificates().and_then(|certs|
            {
                if certs.is_empty()
                {
                    None
                } else
                {
                    Some(certs[0].clone())
                }
            });

        // Turn the warp filter into a service, but instead of using that
        // service directly as usual, we wrap it around another service
        // so that we can modify the request and inject the client certificate
        // into the request extensions before it goes into the filter.
        let mut svc = warp::service(warp_filter.clone());

        let service = service::service_fn(move |mut req|
            {

                if let Some(cert) = client_cert.to_owned()
                {
                    req.extensions_mut().insert(cert);
                }

                // The client socket address is added in the request extensions so that it can be fetched by the handler functions
                req.extensions_mut().insert(client_addr);
                svc.call(req)
            });



        if let Err(e) = hyper::server::conn::Http::new()
            .serve_connection(stream, service)
            .await
        {
            eprintln!("Error handling request: {}", e);
        }
    });

    return Ok(());
}