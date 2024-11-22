import http.server
from pathlib import Path
import socketserver
import json
from urllib.parse import urlparse
from typing import Dict, Any, List
from datetime import datetime
import logging
import requests


# 配置日志
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)
dic: Dict[str, str] = {}
for x in Path("./.env").read_text().splitlines():
    key, value = x.split("=")
    dic[key] = value[1:-1]


class CloudflareD1:
    def __init__(self):
        # 加载环境变量

        """初始化 Cloudflare D1 客户端"""
        self.account_id = dic["CLOUDFLARE_ACCOUNT_ID"]
        self.database_id = dic["CLOUDFLARE_DATABASE_ID"]
        self.api_token = dic["CLOUDFLARE_API_TOKEN"]
        print(111)
        print(self.account_id, self.database_id, self.api_token)

        if not all([self.account_id, self.database_id, self.api_token]):
            raise ValueError("Missing required environment variables")

        self.base_url = f"https://api.cloudflare.com/client/v4/accounts/{self.account_id}/d1/database/{self.database_id}"
        self.headers = {
            "Authorization": f"Bearer {self.api_token}",
            "Content-Type": "application/json",
        }

    def execute_query(self, query: str, params: tuple = ()) -> Dict[str, Any]:
        """执行 SQL 查询"""
        try:
            # 替换参数
            # 注意：这不是最安全的方式，但 D1 API 不支持参数化查询
            for param in params:
                if isinstance(param, str):
                    # 转义单引号
                    param = param.replace("'", "''")
                    query = query.replace("?", f"'{param}'", 1)
                else:
                    query = query.replace("?", str(param), 1)

            response = requests.post(
                f"{self.base_url}/query", headers=self.headers, json={"sql": query}
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            logger.error(f"Database query error: {e}")
            if response:
                logger.error(f"Response: {response.text}")
            raise

    def insert_entry(self, content: str) -> bool:
        """插入新的剪贴板条目"""
        try:
            timestamp = datetime.utcnow().isoformat()
            query = "INSERT INTO clipboard_entries (content, timestamp) VALUES (?, ?)"
            result = self.execute_query(query, (content, timestamp))
            logger.info("Successfully inserted new clipboard entry")
            return True
        except Exception as e:
            logger.error(f"Error inserting clipboard entry: {e}")
            return False

    def get_entries(self, limit: int = 50) -> List[Dict[str, Any]]:
        """获取最近的剪贴板条目"""
        try:
            query = "SELECT content, timestamp FROM clipboard_entries ORDER BY timestamp DESC LIMIT ?"
            result = self.execute_query(query, (limit,))

            # 解析结果
            if (
                "result" in result
                and result["result"]
                and "results" in result["result"][0]
            ):
                return result["result"][0]["results"]
            return []
        except Exception as e:
            logger.error(f"Error fetching clipboard entries: {e}")
            return []


class ClipboardRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        self.db = CloudflareD1()
        super().__init__(*args, **kwargs)

    def do_GET(self):
        """处理 GET 请求"""
        parsed_path = urlparse(self.path)

        if parsed_path.path == "/api/entries":
            self.handle_api_entries()
        elif parsed_path.path == "/":
            self.handle_index()
        else:
            self.send_error(404, "Not Found")

    def do_POST(self):
        """处理 POST 请求"""
        parsed_path = urlparse(self.path)

        if parsed_path.path == "/api/entries":
            content_length = int(self.headers.get("Content-Length", 0))
            post_data = self.rfile.read(content_length)
            try:
                data = json.loads(post_data.decode("utf-8"))
                content = data.get("content")
                if content:
                    success = self.db.insert_entry(content)
                    if success:
                        self.send_json_response({"status": "success"})
                    else:
                        self.send_json_response(
                            {"status": "error", "message": "Failed to insert entry"},
                            500,
                        )
                else:
                    self.send_json_response(
                        {"status": "error", "message": "Content is required"}, 400
                    )
            except json.JSONDecodeError:
                self.send_json_response(
                    {"status": "error", "message": "Invalid JSON"}, 400
                )
        else:
            self.send_error(404, "Not Found")

    def handle_api_entries(self):
        """处理获取条目的 API 请求"""
        entries = self.db.get_entries()
        self.send_json_response({"entries": entries})

    def handle_index(self):
        """处理主页请求"""
        entries = self.db.get_entries()
        html_content = self.generate_html_page(entries)
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(html_content.encode("utf-8"))

    def send_json_response(self, data: Dict[str, Any], status_code: int = 200):
        """发送 JSON 响应"""
        self.send_response(status_code)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(data).encode("utf-8"))

    def generate_html_page(self, entries: List[Dict[str, Any]]) -> str:
        """生成 HTML 页面"""
        entries_html = "\n".join(
            [
                f"""
            <div class="entry">
                <div class="content">{entry['content']}</div>
                <div class="timestamp">{entry['timestamp']}</div>
            </div>
            """
                for entry in entries
            ]
        )

        return f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>剪贴板历史</title>
            <style>
                body {{
                    font-family: Arial, sans-serif;
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }}
                .entry {{
                    border: 1px solid #ddd;
                    margin: 10px 0;
                    padding: 10px;
                    border-radius: 4px;
                }}
                .content {{
                    white-space: pre-wrap;
                    margin-bottom: 5px;
                }}
                .timestamp {{
                    color: #666;
                    font-size: 0.8em;
                }}
            </style>
        </head>
        <body>
            <h1>剪贴板历史</h1>
            <div class="entries">
                {entries_html}
            </div>
        </body>
        </html>
        """


def run_server(host: str = "localhost", port: int = 8000):
    """运行服务器"""
    try:
        with socketserver.TCPServer((host, port), ClipboardRequestHandler) as httpd:
            logger.info(f"Server running at http://{host}:{port}")
            httpd.serve_forever()
    except KeyboardInterrupt:
        logger.info("Server stopped by user")
    except Exception as e:
        logger.error(f"Server error: {e}")
    finally:
        httpd.server_close()


if __name__ == "__main__":
    run_server()

