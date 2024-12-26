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

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    private EditText editTextUsername, editTextPassword;
    private Button buttonLogin;
    private TextView textRegister;

    private final String url = "http://57.180.86.229";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        editTextUsername = findViewById(R.id.editTextUsername);
        editTextPassword = findViewById(R.id.editTextPassword);
        buttonLogin = findViewById(R.id.buttonLogin);
        textRegister = findViewById(R.id.textRegister);

        buttonLogin.setOnClickListener(view -> {
            String username = editTextUsername.getText().toString();
            String password = editTextPassword.getText().toString();

            try {
                if (!username.isEmpty() && !password.isEmpty()){
                    login(username, password);
                }
                else {
                    Toast.makeText(MainActivity.this, "Invalid username or password", Toast.LENGTH_SHORT).show();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        });

        textRegister.setOnClickListener(view -> {
            Intent I = new Intent(MainActivity.this, RegisterActivity.class);
            startActivity(I);
        });
    }

    private void login(String username, String password) throws IOException {
        OkHttpClient client = new OkHttpClient.Builder().build();

        MediaType mediaType = MediaType.parse("application/json; charset=utf-8");

        JSONObject json = new JSONObject();
        try {
            json.put("username", username);
            json.put("password", password);
        } catch (JSONException e) {
            e.printStackTrace();
        }

        RequestBody requestBody = RequestBody.create(json.toString(), mediaType);

        Request request = new Request.Builder()
                .url(url + "/login")
                .post(requestBody)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                runOnUiThread(() ->
                        Toast.makeText(MainActivity.this, "Login failed. Response error", Toast.LENGTH_SHORT).show()
                );
                Log.i("RESPONSE", e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String responseBody = response.body().string();
                if (!response.isSuccessful()){
                    runOnUiThread(() ->
                            Toast.makeText(MainActivity.this, "Login failed. Invalid username or password", Toast.LENGTH_SHORT).show()
                    );
                }
                else {
                    try {
                        JSONObject responseObject = new JSONObject(responseBody);
                        String message = responseObject.getString("message");
                        String token = responseObject.getString("token");
                        runOnUiThread(() ->
                                Toast.makeText(MainActivity.this, "Login successful", Toast.LENGTH_SHORT).show()
                        );
                        Intent I = new Intent(MainActivity.this, HomePageActivity.class);
                        I.putExtra("token", token);
                        startActivity(I);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }
}