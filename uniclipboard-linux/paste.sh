#!/bin/bash
set -eux -o pipefail
URL="http://57.180.86.229"    # 替换为你的 URL
PORT="8080"               # 替换为你的端口
REP=$(curl --silent -d '{"username":"testuser", "password":"test123"}' -H "Content-Type: application/json" -X POST http://57.180.86.229/login)
TOKEN=$(echo "$REP" | jq -r '.token')   # 替换为你的 JWT token


json_string=$(curl -s -X GET \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TOKEN" \
    "$URL/list_all_items" )

# 检查 JSON 字符串是否为空
if [ -z "$json_string" ]; then
  echo "错误：JSON 字符串为空。"
  exit 1
fi

# 使用 jq 提取最新条目。假设 JSON 是一个对象数组。
latest_entry=$(echo "$json_string" | jq -r '.[0].content')

# 如果 jq 找不到任何条目，则报错
if [ -z "$latest_entry" ]; then
  echo "错误：JSON 字符串中没有找到有效条目。"
  exit 1
fi


# 根据操作系统选择剪贴板工具
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  # Linux: 使用 xclip
  if ! command -v xclip &> /dev/null; then
      echo "错误: 未安装 xclip。请先安装它。"
      exit 1
  fi
  echo "$latest_entry" | xclip -in -selection clipboard
  echo "已复制最新条目到剪贴板 (Linux)。"
  exit 0

elif [[ "$OSTYPE" == "darwin"* ]]; then
  # macOS: 使用 pbcopy
  if ! command -v pbcopy &> /dev/null; then
      echo "错误: 未找到 pbcopy。请确认你使用的是 macOS。"
      exit 1
  fi
  echo "$latest_entry" | pbcopy
  echo "已复制最新条目到剪贴板 (macOS)。"

else
  echo "错误：不支持的操作系统。目前支持 Linux 和 macOS。"
  exit 1
fi
exit 0
