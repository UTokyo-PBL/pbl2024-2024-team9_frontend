#!/bin/bash
set -eux -o pipefail
install -vDm 755 -t ~/.local/share/kio/servicemenus/ ./rightclick_menu.desktop
echo "Right-click menu installed successfully."
echo "You can now right-click on a file and select 'Send to Server' to send it to the server."