package com.example.sensordashboard;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ProgressBar;

public class MainActivity extends AppCompatActivity {
    private WebView dashboardWebView;
    private ProgressBar progressBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize WebView and ProgressBar
        dashboardWebView = findViewById(R.id.dashboardWebView);
        progressBar = findViewById(R.id.progressBar);

        // Configure WebView settings
        WebSettings webSettings = dashboardWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        webSettings.setLoadWithOverviewMode(true);
        webSettings.setUseWideViewPort(true);
        webSettings.setSupportZoom(true);
        webSettings.setBuiltInZoomControls(true);
        webSettings.setDisplayZoomControls(false);

        // Handle page load progress
        dashboardWebView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                progressBar.setVisibility(View.GONE);
                super.onPageFinished(view, url);
            }
        });

        // Load Google Looker Studio dashboard
        String dashboardUrl = "https://lookerstudio.google.com/u/8/reporting/373435ee-02f1-40db-8aed-72adb1eaba3b/page/viAFF";
        dashboardWebView.loadUrl(dashboardUrl);
    }

    @Override
    public void onBackPressed() {
        if (dashboardWebView.canGoBack()) {
            dashboardWebView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
