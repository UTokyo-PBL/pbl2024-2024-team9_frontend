#!/bin/bash
set -eux -o pipefail
# 设置变量 (可以从环境变量或者配置文件中读取)
URL="http://57.180.86.229"    # 替换为你的 URL
PORT="8080"               # 替换为你的端口
SEND_DATA=$(cat "$1")
REP=$(curl --silent -d '{"username":"testuser", "password":"test123"}' -H "Content-Type: application/json" -X POST http://57.180.86.229/login)
TOKEN=$(echo "$REP" | jq -r '.token')   # 替换为你的 JWT token
JSON_DATA=$(jq -Rs '{content: .}' "$1") # 设置 JSON 数据

# 检查变量是否为空
if [[ -z "$URL" ]]; then
    echo "Error: URL is not set."
    exit 1
fi

if [[ -z "$TOKEN" ]]; then
    echo "Error: Token is not set."
    exit 1
fi

# 发送 POST 请求
RESPONSE=$(curl -s -X POST \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TOKEN" \
    -d "$JSON_DATA" \
    "$URL/add_item" )

# 获取 HTTP 状态码
STATUS_CODE=$?

# 错误处理
if [ $STATUS_CODE -ne 0 ]; then
    echo "Error: curl command failed with status code $STATUS_CODE"
    exit 1
fi

# 检查 HTTP 状态码
if [[ $RESPONSE =~ "HTTP/1.[01] ([0-9]{3})" ]]; then
  RESPONSE_STATUS_CODE="${BASH_REMATCH[1]}"
  if [[ "$RESPONSE_STATUS_CODE" != "200" ]]; then
    echo "Error: HTTP Status code is $RESPONSE_STATUS_CODE"
    echo "Response body: $RESPONSE"
    exit 1
  fi
fi

echo "POST request sent successfully."
echo "Response body: $RESPONSE"