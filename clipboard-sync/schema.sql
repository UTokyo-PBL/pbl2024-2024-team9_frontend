-- Create the clipboard_entries table
CREATE TABLE IF NOT EXISTS clipboard_entries (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content TEXT NOT NULL,
    timestamp TEXT NOT NULL
);

-- Create an index on timestamp for better query performance
CREATE INDEX IF NOT EXISTS idx_clipboard_timestamp ON clipboard_entries(timestamp);
