#!/bin/bash
set -eux -o pipefail
# 设置目标 URL
TARGET_URL="http://57.180.86.229/add_item" # 将此替换为您想要发送数据的 URL
TOKEN="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoiNjc2NThmODVlZmY3M2ZkODE2YzBjMjdkIiwiZXhwIjoxNzM1MjQwMjY3fQ.WTymwYQi5L2iyVBZyHPI2UkVxzMlX0E96xfqLkUgj9o"   # 替换为你的 JWT token

# 获取剪贴板内容 (使用 xclip 或 pbpaste, 根据您的系统选择)
if command -v xclip &> /dev/null; then
    CLIPBOARD_CONTENT=$(xclip -o -selection clipboard)
    JSON_DATA=$(jq -n --arg key "content" --arg value "$CLIPBOARD_CONTENT" '{$key: $value}')
elif command -v pbpaste &> /dev/null; then
    CLIPBOARD_CONTENT=$(pbpaste)
    JSON_DATA=$(jq -n --arg key "content" --arg value "$CLIPBOARD_CONTENT" '{$key: $value}') # 设置 JSON 数据
else
    echo "错误：未找到 xclip 或 pbpaste。请安装一个以访问剪贴板。"
    exit 1
fi

# 使用 curl 发送 POST 请求
curl -X POST \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TOKEN" \
    -d "$JSON_DATA" \
    "$TARGET_URL"

# 检查 curl 命令是否成功执行
if [ $? -eq 0 ]; then
    echo "剪贴板内容已成功发送到 $TARGET_URL"
else
    echo "发送剪贴板内容时出错。"
fi
