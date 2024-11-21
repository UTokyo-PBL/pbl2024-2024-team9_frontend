use std::time::Duration;
use arboard::Clipboard;
use tokio::time;
use serde::{Deserialize, Serialize};
use anyhow::{Result, Context, anyhow};
use chrono::Utc;

#[derive(Debug, Serialize, Deserialize)]
struct ClipboardEntry {
    content: String,
    timestamp: String,
}

struct CloudflareClient {
    client: reqwest::Client,
    account_id: String,
    database_id: String,
    api_token: String,
}

impl CloudflareClient {
    fn new(account_id: String, database_id: String, api_token: String) -> Self {
        let client = reqwest::Client::new();
        Self {
            client,
            account_id,
            database_id,
            api_token,
        }
    }

    async fn insert_clipboard(&self, entry: &ClipboardEntry) -> Result<()> {
        let url = format!(
            "https://api.cloudflare.com/client/v4/accounts/{}/d1/database/{}/query",
            self.account_id, self.database_id
        );

        let query = format!(
            "INSERT INTO clipboard_entries (content, timestamp) VALUES ('{}', '{}')",
            entry.content.replace("'", "''"), // Escape single quotes
            entry.timestamp
        );

        let response = self.client
            .post(&url)
            .header("Authorization", format!("Bearer {}", self.api_token))
            .json(&serde_json::json!({
                "sql": query,
            }))
            .send()
            .await
            .with_context(|| "Failed to send request to Cloudflare")?;

        if !response.status().is_success() {
            let error_text = response.text().await?;
            return Err(anyhow!("Failed to insert data: {}", error_text));
        }

        Ok(())
    }
}

async fn monitor_clipboard(cf_client: CloudflareClient) -> Result<()> {
    let mut ctx = Clipboard::new()?;
    let mut last_content = ctx.get_text()
        .with_context(|| "Failed to get initial clipboard contents")?;

    println!("Started monitoring clipboard...");

    let mut interval = time::interval(Duration::from_secs(1));
    loop {
        interval.tick().await;
        
        match ctx.get_text() {
            Ok(current_content) => {
                if current_content != last_content {
                    println!("New clipboard content detected!");
                    
                    let entry = ClipboardEntry {
                        content: current_content.clone(),
                        timestamp: Utc::now().to_rfc3339(),
                    };

                    if let Err(e) = cf_client.insert_clipboard(&entry).await {
                        eprintln!("Failed to sync to D1: {:#}", e);
                    } else {
                        println!("Successfully synced to D1!");
                    }

                    last_content = current_content;
                }
            }
            Err(e) => {
                eprintln!("Failed to get clipboard contents: {}", e);
            }
        }
    }
}

#[tokio::main]
async fn main() -> Result<()> {
    dotenv::dotenv().ok();

    let account_id = std::env::var("CLOUDFLARE_ACCOUNT_ID")
        .with_context(|| "CLOUDFLARE_ACCOUNT_ID not set")?;
    let database_id = std::env::var("CLOUDFLARE_DATABASE_ID")
        .with_context(|| "CLOUDFLARE_DATABASE_ID not set")?;
    let api_token = std::env::var("CLOUDFLARE_API_TOKEN")
        .with_context(|| "CLOUDFLARE_API_TOKEN not set")?;

    let cf_client = CloudflareClient::new(
        account_id,
        database_id,
        api_token,
    );

    monitor_clipboard(cf_client).await
}