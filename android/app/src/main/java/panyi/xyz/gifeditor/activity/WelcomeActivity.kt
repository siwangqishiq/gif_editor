package panyi.xyz.gifeditor.activity

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import panyi.xyz.gifeditor.R
import panyi.xyz.gifeditor.NativeBridge
import panyi.xyz.gifeditor.data.PICKER_TYPE_VIDEO
import panyi.xyz.gifeditor.data.REQUEST_CODE_PICK_VIDEO
import panyi.xyz.gifeditor.data.REQUEST_PERMISSION_READ_VIDEOS
import panyi.xyz.gifeditor.util.FileUtil
import xyz.panyi.textrender.util.Log
import java.net.URI

class WelcomeActivity : AppCompatActivity() {
    companion object{
        const val TAG = "WelcomeActivity"
    }
    
    private lateinit var versionText: TextView
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_welcome)
        initView()
        versionText.text = ("ffmpeg version: " + NativeBridge.ffmpegVersion())
    }
    
    private fun initView(){
        findViewById<View>(R.id.btn_select_video).setOnClickListener{
            Log.i(TAG, "select video button clicked.")
            selectVideoFile()
        }
        versionText = findViewById<TextView>(R.id.text_ff_version)
    }
    
    private fun selectVideoFile(){
        Log.i(TAG, "selectVideoFile")
        
        var reqPerm :String? = null
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU){
            reqPerm = Manifest.permission.READ_MEDIA_VIDEO
        }else{
            reqPerm = Manifest.permission.READ_EXTERNAL_STORAGE
        }
        
        if(checkSelfPermission(reqPerm) == PackageManager.PERMISSION_GRANTED){
            FilePickerActivity.start(this, PICKER_TYPE_VIDEO, REQUEST_CODE_PICK_VIDEO)
        }else{
            requestPermissions(arrayOf(reqPerm), REQUEST_PERMISSION_READ_VIDEOS)
        }
    }
    
    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        
        if(requestCode == REQUEST_PERMISSION_READ_VIDEOS){
            if(grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                FilePickerActivity.start(this, PICKER_TYPE_VIDEO, REQUEST_CODE_PICK_VIDEO)
            }
        }
    }
    
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_CODE_PICK_VIDEO && resultCode == RESULT_OK) {
            val selectedVideos = data?.getParcelableArrayExtra(FilePickerActivity.RESULT_SELECTED_FILES)
                ?.filterIsInstance<Uri>()
                ?.toList() ?: emptyList()
            // 处理选择的视频
            if(selectedVideos.isNotEmpty()){
                Log.i(TAG, "select video file: ${selectedVideos[0]}")
                val filepath = FileUtil.convertUriToPath(this, selectedVideos[0])
                Log.i(TAG, "select video file path: $filepath")
                MainActivity.start(this, filepath)
            }else{
                Log.e(TAG, "selectedVideos is Empty!")
            }
            
        }
        
    }
}//end class