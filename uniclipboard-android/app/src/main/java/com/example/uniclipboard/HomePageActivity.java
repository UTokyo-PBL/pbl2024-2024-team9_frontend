package com.example.uniclipboard;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.OpenableColumns;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.TimeUnit;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class HomePageActivity extends AppCompatActivity {

    private TextView textview1, textview2, textview3;
    private EditText editText;
    private Button buttonCopy, buttonCopyFile;

    private TextView[] textViews;

    private final String label = "From UniClipboard";
    private final String url = "http://57.180.86.229";
    private String token;
    private Item[] recentItems;

    private final ActivityResultLauncher<Intent> pickFileLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == RESULT_OK) {
                    Intent resultData = result.getData();
                    if (resultData != null) {
                        Uri uri = resultData.getData();
                        Cursor cursor = getContentResolver().query(uri, null, null, null, null);
                        if (cursor != null) {
                            int columnIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                            if (columnIndex != -1) {
                                cursor.moveToFirst();
                                String fileName = cursor.getString(columnIndex);
                                try (BufferedInputStream fileInputStream = new BufferedInputStream(getContentResolver().openInputStream(uri))) {
                                    upload(fileInputStream, fileName);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                            cursor.close();
                        }
                    }
                }
            }
    );


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
        buttonCopy = findViewById(R.id.buttonCopyText);
        buttonCopyFile = findViewById(R.id.buttonCopyFile);

        textViews = new TextView[] {textview1, textview2, textview3};
        recentItems = new Item[] {new Item(), new Item(), new Item()};

        token = getIntent().getStringExtra("token");

        try {
            getItemLists();
        } catch (IOException e){
            e.printStackTrace();
        }

        buttonCopy.setOnClickListener(view -> {
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

        buttonCopyFile.setOnClickListener(view -> {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            String[] mimeTypes = {
                    "text/plain",
                    "text/html",
                    "text/csv",
                    "text/x-c",
                    "text/x-java",
                    "text/x-python",
                    "text/javascript",
                    "image/png",
                    "image/jpeg",
                    "image/gif",
                    "audio/mpeg",
                    "audio/flac",
                    "audio/wav",
                    "application/x-sh",
                    "application/x-csh",
                    "application/java-archive",
                    "application/zip",
                    "application/gzip",
                    "application/vnd.rar",
                    "application/x-7z-compressed",
                    "application/xml",
                    "application/pdf",
                    "application/vnd.ms-excel",
                    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
                    "application/msword",
                    "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
                    "application/vnd.ms-powerpoint",
                    "application/vnd.openxmlformats-officedocument.presentationml.presentation",
                    "application/octet-stream",
                    "application/json"
            };
            intent.putExtra(Intent.EXTRA_MIME_TYPES, mimeTypes);
            pickFileLauncher.launch(intent);
        });

        for (int i = 0; i < 3; i++){
            int index = i;
            textViews[i].setOnClickListener(view -> {
                if (recentItems[index].isFile()) {
                    try {
                        download(recentItems[index].getId(), recentItems[index].getText());
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                else {
                    ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                    ClipData clip = ClipData.newPlainText(label, textViews[index].getText());
                    clipboard.setPrimaryClip(clip);
                }
            });
        }
    }


    private void upload(InputStream fileInputStream, String fileName) throws IOException {
        OkHttpClient client = new OkHttpClient.Builder().build();

        MediaType mediaType = MediaType.parse("application/octet-stream");

        RequestBody requestBody = new MultipartBody.Builder()
                .setType(MultipartBody.FORM)
                .addFormDataPart("file", fileName,
                        RequestBody.create(fileInputStream.readAllBytes(), mediaType))
                .build();

        Request request = new Request.Builder()
                .header("Authorization","Bearer " + token)
                .url(url + "/upload_file")
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
                            Toast.makeText(HomePageActivity.this, "Upload file successful", Toast.LENGTH_SHORT).show()
                    );
                    getItemLists();
                }
            }
        });
    }

    private void getItemLists() throws IOException {
        OkHttpClient client = new OkHttpClient.Builder().build();

        Request request = new Request.Builder()
                .header("Authorization","Bearer " + token)
                .url(url + "/list_all_files_and_items")
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
                        for (int i = 0; i < items.length(); i++) {
                            JSONObject item = items.getJSONObject(i);
                            Item recentItem = recentItems[i];
                            recentItem.clear();
                            recentItem.setType(item.getString("type"));
                            if (recentItem.isFile()) {
                                recentItem.setText(item.getString("filename"));
                                recentItem.setId(item.getString("_id"));
                                int index = i;
                                runOnUiThread(() -> {
                                    textViews[index].setTypeface(Typeface.defaultFromStyle(Typeface.BOLD));
                                    textViews[index].setPaintFlags(Paint.UNDERLINE_TEXT_FLAG);
                                    }
                                );
                            }
                            else {
                                recentItem.setText(item.getString("content"));
                            }
                            textViews[i].setText(recentItem.getText());
                            if (i == 2){
                                break;
                            }
                        }
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
                            Toast.makeText(HomePageActivity.this, "Copy text successful", Toast.LENGTH_SHORT).show()
                    );
                    getItemLists();
                }
            }
        });
    }

    private void download(String fileId, String fileName) throws IOException {
        OkHttpClient client = new OkHttpClient.Builder().build();

        Request request = new Request.Builder()
                .header("Authorization", "Bearer " + token)
                .url(url + "//download_file/" + fileId)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                Log.i("RESPONSE", e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                String responseBody = response.body().string();
                if (!response.isSuccessful()) {
                    Log.i("RESPONSE", responseBody);
                } else {
                    try {
                        JSONObject responseObject = new JSONObject(responseBody);
                        String downloadUrl = responseObject.getString("download_url");
                        downloadFromUrl(downloadUrl, fileName);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    private void downloadFromUrl(String downloadUrl, String fileName) throws IOException {
        OkHttpClient client = new OkHttpClient.Builder()
                .connectTimeout(60, TimeUnit.SECONDS)
                .readTimeout(60, TimeUnit.SECONDS)
                .writeTimeout(60, TimeUnit.SECONDS)
                .build();

        Request request = new Request.Builder()
                .url(downloadUrl)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                Log.i("RESPONSE", e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (!response.isSuccessful()) {
                    runOnUiThread(() ->
                            Toast.makeText(HomePageActivity.this, "Download file failed", Toast.LENGTH_SHORT).show()
                    );
                } else {
                    File file = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), fileName);
                    try (BufferedInputStream  inputStream = new BufferedInputStream(response.body().byteStream());
                         FileOutputStream outputStream = new FileOutputStream(file)) {
                        byte[] buffer = new byte[1024];
                        int read;
                        while ((read = inputStream.read(buffer)) != -1) {
                            outputStream.write(buffer, 0, read);
                        }
                        runOnUiThread(() ->
                                Toast.makeText(HomePageActivity.this, "Download file successful", Toast.LENGTH_SHORT).show()
                        );
                    } catch (IOException e){
                        e.printStackTrace();
                    }
                }
            }
        });
    }
}