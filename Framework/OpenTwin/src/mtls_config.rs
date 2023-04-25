use tokio_rustls::TlsAcceptor;
use std::{fs, io, sync, vec::Vec};
use rustls::internal::pemfile;

pub async fn get_mtls_acceptor(server_cert_path: &str, server_key_path: &str, root_store_path: &str) -> TlsAcceptor
{
    // Build TLS configuration.
    let tls_config =
        {
            // Load server key pair
            let (certs, key) = load_keypair(server_cert_path, server_key_path).unwrap();

            // Load root CA certificate file used to verify client certificate
            let rootstore = load_root_store(root_store_path).unwrap();

            let mut config = rustls::ServerConfig::new(
                rustls::AllowAnyAnonymousOrAuthenticatedClient::new(rootstore),
            );

            // Setting the Certificate that will be used for this specific server
            config
                .set_single_cert(certs, key)
                .map_err(|err| io::Error::new(io::ErrorKind::Other, format!("{}", err))).unwrap();


            sync::Arc::new(config)
        };

    tokio_rustls::TlsAcceptor::from(tls_config)
}


fn load_keypair(
    certfile: &str,
    keyfile: &str,
) -> io::Result<(Vec<rustls::Certificate>, rustls::PrivateKey)>
{
    // Opening the files to read
    let file = fs::File::open(certfile)?;
    let mut reader = io::BufReader::new(file);

    let certs = pemfile::certs(&mut reader).map_err(|_err|
        {
            io::Error::new(
                io::ErrorKind::Other,
                format!("Cannot load certificate from {}", certfile),
            )
        })?;

    let file = fs::File::open(keyfile)?;
    let mut reader = io::BufReader::new(file);

    // Load and return a single private key.
    let keys = pemfile::rsa_private_keys(&mut reader).map_err(|_err|
        {
            io::Error::new(
                io::ErrorKind::Other,
                format!("Cannot load private key from {}", keyfile),
            )
        })?;

    let key = keys.first().ok_or(io::Error::new(
        io::ErrorKind::Other,
        format!("No keys found in the private key file {}", keyfile),
    ))?;

    Ok((certs, key.clone()))
}

fn load_root_store(filename: &str) -> io::Result<rustls::RootCertStore>
{

    let file = fs::File::open(filename)?;
    let mut reader = io::BufReader::new(file);

    let mut store = rustls::RootCertStore::empty();
    store.add_pem_file(&mut reader).map_err(|_err|
        {
            io::Error::new(
                io::ErrorKind::Other,
                format!("Failed to load root cert store from {}", filename),
            )
        })?;

    Ok(store)
}