package panyi.xyz.gifeditor.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import panyi.xyz.gifeditor.R;

public class TestActivity extends AppCompatActivity {
    public static void start(Context ctx){
        Intent intent = new Intent(ctx, TestActivity.class);
        ctx.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);
    }

}//end class