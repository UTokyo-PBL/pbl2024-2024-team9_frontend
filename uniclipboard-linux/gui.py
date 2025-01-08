import os
import subprocess
import tkinter as tk
import tkinter.ttk as ttk


def run_script(script_name: str):
    """运行指定名称的脚本"""
    try:
        # 获取当前脚本所在目录
        current_dir = os.path.dirname(os.path.abspath(__file__))
        script_path = os.path.join(current_dir, script_name)

        # 检查脚本是否存在
        if not os.path.exists(script_path):
            _ = message_label.config(text=f"Error: Script '{script_name}' not found.")
            return

        # 检查脚本是否有执行权限
        if not os.access(script_path, os.X_OK):
            _ = message_label.config(
                text=f"Error: Script '{script_name}' does not have execute permission."
            )
            return

        # 运行脚本
        result = subprocess.run(
            [script_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=2,
        )

        if result.returncode == 0:
            _ = message_label.config(
                text=f"Script '{script_name}' completed successfully."
            )
            output_text.delete(1.0, tk.END)  # 清空之前的内容
            output_text.insert(tk.END, result.stdout)  # 将输出显示出来
        else:
            _ = message_label.config(
                text=f"Error: Script '{script_name}' failed with exit code {result.returncode}"
            )
            output_text.delete(1.0, tk.END)  # 清空之前的内容
            output_text.insert(tk.END, result.stderr)  # 将错误信息显示出来
    except Exception as e:
        _ = message_label.config(text=f"An unexpected error occurred: {e}")


# 创建主窗口
window = tk.Tk()
window.title("Script Runner")
window.geometry("600x400")

# 创建标题标签
title_label = ttk.Label(window, text="Run Scripts", font=("Arial", 16))
title_label.pack(pady=10)

# 创建一个frame用来放按钮
button_frame = ttk.Frame(window)
button_frame.pack(pady=10)

# 创建按钮并绑定脚本
script_buttons = {
    "clip_board.sh": "send clipboard content to cloud",
    "get_all_item.sh": "get all items from cloud",
    "install_rightclick_menu.sh": "install right click menu",
    "paste.sh": "paste content from cloud",
}

for script_name, button_text in script_buttons.items():
    button = ttk.Button(
        button_frame,
        text=button_text,
        command=lambda name=script_name: run_script(name),
    )
    button.pack(side=tk.LEFT, padx=10)  # 按钮并排显示

# 创建消息标签
message_label = tk.Label(window, text="", fg="red")
message_label.pack(pady=5)

# 创建输出文本框
output_text = tk.Text(window, height=10)
output_text.pack(pady=10, padx=10, fill=tk.BOTH, expand=True)

# 启动主循环
window.mainloop()
