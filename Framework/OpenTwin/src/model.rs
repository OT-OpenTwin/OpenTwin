use serde::{Deserialize, Serialize};

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct PerformAction {
    pub action: String,
    pub model_id: Option<u32>,
    pub text: Option<String>,
    pub visualization_model_id: Option<u32>,
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct QueueAction {
    pub action: String,
    pub model_id: Option<u32>,
    pub text: Option<String>,
    pub visualization_model_id: Option<u32>,
}
