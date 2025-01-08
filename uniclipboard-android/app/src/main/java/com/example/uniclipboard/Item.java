package com.example.uniclipboard;

public class Item {
    private String text;
    private String id;
    private boolean isTypeFile;

    public Item() {
    }

    public void setText(String text) {
        this.text = text;
    }

    public void setId(String id) {
        this.id = id;
    }

    public boolean isFile() {
        return this.isTypeFile;
    }

    public String getText() {
        return this.text;
    }

    public String getId() {
        return this.id;
    }

    public void setType(String type) {
        if (type.equals("file")){
            this.isTypeFile = true;
        };
    }

    public void clear() {
        text = "";
        id = "";
        isTypeFile = false;
    }


}
