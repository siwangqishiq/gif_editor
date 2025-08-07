package panyi.xyz.gifeditor.activity

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import panyi.xyz.gifeditor.R
import panyi.xyz.gifeditor.NativeBridge
import panyi.xyz.gifeditor.data.PICKER_TYPE_VIDEO
import xyz.panyi.fullstackeditor.data.REQUEST_CODE_PICK_VIDEO
import xyz.panyi.fullstackeditor.data.REQUEST_PERMISSION_READ_VIDEOS
import xyz.panyi.textrender.util.Log

class WelcomeActivity : AppCompatActivity() {
    companion object{
        const val TAG = "MainActivity"
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
        if(checkSelfPermission(Manifest.permission.READ_MEDIA_VIDEO) == PackageManager.PERMISSION_GRANTED){
            FilePickerActivity.start(this, PICKER_TYPE_VIDEO, REQUEST_CODE_PICK_VIDEO)
        }else{
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                requestPermissions(arrayOf(Manifest.permission.READ_MEDIA_VIDEO), REQUEST_PERMISSION_READ_VIDEOS)
            }else{
                requestPermissions(arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE), REQUEST_PERMISSION_READ_VIDEOS)
            }
        }
    }
    
    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        
        if(requestCode == REQUEST_PERMISSION_READ_VIDEOS){
            if(grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED){
//                FilePickerActivity.start(this, PICKER_TYPE_VIDEO, REQUEST_CODE_PICK_VIDEO)
            }
        }
    }
    
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_CODE_PICK_VIDEO && resultCode == RESULT_OK) {
//            val selectedVideos = data?.getParcelableArrayExtra(FilePickerActivity.RESULT_SELECTED_FILES)
//                ?.filterIsInstance<Uri>()
//                ?.toList() ?: emptyList()
//
//            // 处理选择的视频
//            if(selectedVideos.isNotEmpty()){
//                Log.i(TAG, "select video file: ${selectedVideos[0]}")
//            }else{
//                Log.e(TAG, "selectedVideos is Empty!")
//            }
            
        }
        
    }
}//end class