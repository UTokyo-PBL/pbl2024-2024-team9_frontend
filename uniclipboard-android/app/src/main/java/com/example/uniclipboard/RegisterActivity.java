package com.example.uniclipboard;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
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

public class RegisterActivity extends AppCompatActivity {

    private EditText editTextUsername, editTextPassword, editTextReenter;
    private Button buttonRegister;

    private final String url = "http://57.180.86.229";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_register);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.register), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        editTextUsername = findViewById(R.id.editTextUsername);
        editTextPassword = findViewById(R.id.editTextPassword);
        editTextReenter = findViewById(R.id.editTextReenter);
        buttonRegister = findViewById(R.id.buttonRegister);

        buttonRegister.setOnClickListener(view -> {
            String username = editTextUsername.getText().toString();
            String password = editTextPassword.getText().toString();
            String reenter = editTextReenter.getText().toString();

            try {
                if (username.isEmpty() || password.isEmpty()
                        || reenter.isEmpty()){
                    Toast.makeText(RegisterActivity.this, "Invalid username or password", Toast.LENGTH_SHORT).show();
                }
                else {
                    if (!password.equals(reenter)){
                        Toast.makeText(RegisterActivity.this, "Passwords do not match", Toast.LENGTH_SHORT).show();
                    }
                    else {
                        register(username, password);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
    }

    private void register(String username, String password) throws IOException {
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
                .url(url + "/register")
                .post(requestBody)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                runOnUiThread(() ->
                        Toast.makeText(RegisterActivity.this, "Register failed. Response error", Toast.LENGTH_SHORT).show()
                );
                Log.i("RESPONSE", e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (!response.isSuccessful()){
                    runOnUiThread(() ->
                            Toast.makeText(RegisterActivity.this, "Register failed. Invalid username or password", Toast.LENGTH_SHORT).show()
                    );
                }
                else {
                    runOnUiThread(() ->
                            Toast.makeText(RegisterActivity.this, "Register successful", Toast.LENGTH_SHORT).show()
                    );
                    Intent I = new Intent(RegisterActivity.this, MainActivity.class);
                    startActivity(I);
                }
            }
        });
    }
}
