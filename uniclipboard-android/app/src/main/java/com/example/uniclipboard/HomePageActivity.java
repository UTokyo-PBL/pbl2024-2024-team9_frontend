package com.example.uniclipboard;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class HomePageActivity extends AppCompatActivity {

    private TextView textview1, textview2, textview3;
    private EditText editText;
    private Button buttonPaste;

    private final String url = "http://57.180.86.229";
    private String token;
    private String[] texts = new String[3];


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_home_page);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.home), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        textview1 = findViewById(R.id.textView1);
        textview2 = findViewById(R.id.textView2);
        textview3 = findViewById(R.id.textView3);
        editText = findViewById(R.id.editText);
        buttonPaste = findViewById(R.id.buttonPaste);

        token = getIntent().getStringExtra("token");

        try{
            getItemLists();
        } catch (IOException e){
            e.printStackTrace();
        }

        buttonPaste.setOnClickListener(view -> {
            String text = editText.getText().toString();
            if (!text.isEmpty()) {
                try {
                    addItem(text);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {
                Toast.makeText(HomePageActivity.this, "Invalid text", Toast.LENGTH_SHORT).show();
            }
        });

    }

    private void getItemLists() throws IOException {
        OkHttpClient client = new OkHttpClient.Builder().build();

        Request request = new Request.Builder()
                .header("Authorization","Bearer " + token)
                .url(url + "/list_all_items")
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                Log.i("RESPONSE", e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String responseBody = response.body().string();
                if (!response.isSuccessful()){
                    Log.i("RESPONSE", responseBody);
                }
                else {
                    try {
                        JSONArray items = new JSONArray(responseBody);
                        texts = new String[3];
                        for (int i = 0; i < items.length(); i++){
                            JSONObject item = items.getJSONObject(i);
                            texts[i] = item.getString("content");
                            if (i == 2){
                                break;
                            }
                        }
                        textview1.setText(texts[0]);
                        textview2.setText(texts[1]);
                        textview3.setText(texts[2]);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    private void addItem(String text) throws IOException{
        OkHttpClient client = new OkHttpClient.Builder().build();

        MediaType mediaType = MediaType.parse("application/json; charset=utf-8");

        JSONObject json = new JSONObject();
        try {
            json.put("content", text);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        RequestBody requestBody = RequestBody.create(json.toString(), mediaType);

        Request request = new Request.Builder()
                .header("Authorization","Bearer " + token)
                .url(url + "/add_item")
                .post(requestBody)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                Log.i("RESPONSE", e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String responseBody = response.body().string();
                if (!response.isSuccessful()){
                    Log.i("RESPONSE", responseBody);
                }
                else {
                    runOnUiThread(() ->
                            Toast.makeText(HomePageActivity.this, "Paste successful", Toast.LENGTH_SHORT).show()
                    );
                    getItemLists();
                }
            }
        });
    }
}